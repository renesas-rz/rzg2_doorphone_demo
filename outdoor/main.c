/***********************************************************************
 * FILENAME: main.c
 *
 * DESCRIPTION:
 *   Main function.
 * 
 * AUTHOR: RVC       START DATE: 25/12/2019
 *
 * CHANGES:
 * 
 ***********************************************************************/

/* ---------- Header files ---------- */
#include <glib.h>
#include <glib/gprintf.h>

#include <stdlib.h>
#include <libgen.h>

#include <gst/rtsp-server/rtsp-server.h>

#include "camera.h"
#include "my_gst.h"
#include "helper.h"
#include "param.h"

/*
 * Function: main
 * ----------------------------
 *   I. Purpose: 
 *     1. Setup RTSP server.
 *     2. Send camera streams (MIPI/USB camera) to client.
 *
 *   II. Notes:
 *     1. If MIPI camera is detected, it will be the main camera.
 *     2. By default, it will accept up to 4 cameras. MIPI will always be accepted if detected.
 *     3. If the number of camera is not enough, it will:
 *       - Raises a warning, then continue to stream detected cameras.
 *       - If not enough, it will find and stream video samples inside a pre-defined directory.
 *       - If not enough, it will find up the slots by sending test screen (generated from videotestsrc element)
 *     4. Below is the streaming priority:
 *       - MIPI camera: the default resolution should be 1280x960.
 *       - USB camera: it will stream from top to the bottom. Do not care the quality of the camera (resolution...). The default resolution should be 1280x720.
 *       - Test screen
 * 
 *   argc: Number of arguments passed in this program.
 *   argv: Arguments' values.
 * 
 *   returns: always 0 (run successfully).
 */

int main(int argc, char *argv[])
{
    /* Contains error messages during parsing arguments */
    gchar error[100];

    /* Main loop */
    GMainLoop *loop = NULL;

    /* RTSP server */
    GstRTSPServer *server = NULL;

    /* Used to set port for RTSP servers */
    gchar *port_str = NULL;

    /* List of ports for RTSP servers */
    gint *ports = NULL;
    gint port_counts = 0;

    /* List of collected cameras */
    struct camera_t **cameras = NULL;
    gint camera_size = 0;

    /* Camera resolution */
    gchar *width = (char *) malloc(sizeof(char*) * 10);
    gchar *height = (char *) malloc(sizeof(char*) * 10);;

    gint index = 0;

    /* GStreamer pipeline */
    gchar pipeline[500];

    /* RTSP server's objects */
    GstRTSPMountPoints *mounts = NULL;
    GstRTSPMediaFactory *factory = NULL;

    /* Try to parse parameters */
    if (!param_parse(&argc, &argv, error))
    {
        g_message("Error: Failed to parse option: %s", error);
        param_free();

        exit(1);
    }

    /* Exit the app if users would like to know the version */
    if (param_is_version_enabled())
    {
        g_message("%s %s", basename(argv[0]), param_get_version());
        param_free();

        exit(0);
    }

    /* Print out parameters */
    param_print_all();

    /* Create main loop */
    loop = g_main_loop_new(NULL, FALSE);

    /* Get ports for RTSP servers */
    param_get_rtsp_server_ports(&ports, &port_counts);

    /* Get collected cameras (including MIPI, USB, fake cameras).
     * The output of this function will always be reliable at this point. */
    param_get_cameras(&cameras, &camera_size);

    /* Get resolution of camera */
    param_get_resolution(width, height);

    /* For each camera, create a pipeline from it, then attach it to "GstRTSPMountPoints" */
    for (index = 0; index < camera_size; index++)
    {
        /* Create RTSP server */
        server = gst_rtsp_server_new();

        /* Set port for RTSP server
         *
         * We needn't set IP address for it. By default, it will listen for incoming
         * connecting from address 0.0.0.0. In the context of servers, 0.0.0.0 means all
         * IPv4 addresses on the local machine. If a host has two IP addresses, 192.168.1.1
         * and 10.1.2.1, and a server running on the host listens on 0.0.0.0,
         * it will be reachable at both of those IPs.*/
        port_str = g_strdup_printf("%d", ports[index]);
        gst_rtsp_server_set_service(server, port_str);

        /* Get default GstRTSPMountPoints from "server" */
        mounts = gst_rtsp_server_get_mount_points(server);

        /* Create pipeline */
        if(gst_get_camera_pipeline(cameras[index], pipeline, width, height) == TRUE)
        {
            /* Create a new GstRTSPMediaFactory instance */
            factory = gst_rtsp_media_factory_new();

            /* Create an RTP feed of a camera playback */
            gst_rtsp_media_factory_set_launch(factory, pipeline);

            /* Share the pipeline between clients. If it is not set, both USB and
             * MIPI camera will fail if a client tries to access their URLs when they are
             * being used by other client */
            gst_rtsp_media_factory_set_shared(factory, TRUE);

            /* Attach the pipeline to new URL */
            gst_rtsp_mount_points_add_factory(mounts, "/camera", factory);

            /* Attach the server to the default main context */
            gst_rtsp_server_attach (server, NULL);

            g_message("Stream is ready at: \"rtsp://<IP address>:%s/camera\"", port_str);
        } else {
            return -1;
        }

        /* Don't need the ref to the mapper anymore */
        g_object_unref (mounts);

        /* Free resources */
        g_free(port_str);
    }

    /* Start main loop */
    g_main_loop_run (loop);

    /* De-initialize variables */
    param_free();

    return 0;
}
