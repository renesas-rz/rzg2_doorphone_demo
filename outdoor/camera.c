/***********************************************************************
 * FILENAME: camera.c
 *
 * DESCRIPTION:
 *   Camera implementations.
 *
 * NOTE:
 *   For more further information about datatypes and function usages,
 *   please refer to "camera_dev.h".
 *
 * AUTHOR: RVC       START DATE: 30/12/2019
 *
 * CHANGES:
 *
 ***********************************************************************/

/* ---------- Header files ---------- */

#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "camera.h"

/* ---------- Datatypes ---------- */

union camera_id_t
{
    gchar fd[20];
    gchar file_path[100];
};

struct camera_t
{
    enum camera_type_t type;
    union camera_id_t id;
};

/* ---------- Macros ---------- */

#define RZG2E_MIPI_CAM_LIMIT_INPUT_WIDTH 640
#define RZG2E_MIPI_CAM_LIMIT_INPUT_HEIGHT 480
#define RZG2E_MIPI_CAM_LIMIT_ENC_BITRATE 2500000

#define RZG2E_USB_CAM_LIMIT_INPUT_WIDTH 640
#define RZG2E_USB_CAM_LIMIT_INPUT_HEIGHT 480
#define RZG2E_USB_CAM_LIMIT_ENC_BITRATE 2000000

#define RZG2M_MIPI_CAM_LIMIT_INPUT_WIDTH 1280
#define RZG2M_MIPI_CAM_LIMIT_INPUT_HEIGHT 960
#define RZG2M_MIPI_CAM_LIMIT_ENC_BITRATE 4000000

#define RZG2M_USB_CAM_LIMIT_INPUT_WIDTH 1280
#define RZG2M_USB_CAM_LIMIT_INPUT_HEIGHT 720
#define RZG2M_USB_CAM_LIMIT_ENC_BITRATE 4000000

#define RZG2N_MIPI_CAM_LIMIT_INPUT_WIDTH 1280
#define RZG2N_MIPI_CAM_LIMIT_INPUT_HEIGHT 960
#define RZG2N_MIPI_CAM_LIMIT_ENC_BITRATE 4000000

#define RZG2N_USB_CAM_LIMIT_INPUT_WIDTH 1280
#define RZG2N_USB_CAM_LIMIT_INPUT_HEIGHT 720
#define RZG2N_USB_CAM_LIMIT_ENC_BITRATE 4000000

/* ---------- Private functions ---------- */

/*
 * Function: supported_platform_get_index
 * ---
 *   Get index of "supported_platforms" array.
 *
 *   return: index >= 0 if host name exists in the array.
 *           index == -1 if host name does not exist in the array.
 */
static gint supported_platform_get_index();

/* ---------- Private variables ---------- */

/* The following code is based on document "R01US0424EJ0102_VideoCapture_UME_v1.02_06.pdf" */

/* Commands to initialize MIPI camera on RZ/G2E */
const gchar *rzg2e_mipi_init_steps[] = 
{
    /* Reset all links before enabling new links */
    "media-ctl -d /dev/media0 -r",

    /* Link "VIN4" to "CSI40/VC0". After this, we can start capturing video data from "/dev/video0"
     * which is associated with "VIN4" */
    "media-ctl -d /dev/media0 -l \"'rcar_csi2 feaa0000.csi2':1 -> 'VIN4 output':0 [1]\"",

    /* Set the same format as camera "OV5645" */
    "media-ctl -d /dev/media0 -V \"'rcar_csi2 feaa0000.csi2':1 [fmt:UYVY8_2X8/1280x960 field:none]\"",

    /* Set data format "UYVY8_2X8" and resolution "1280x960" to camera "OV5645" */
    "media-ctl -d /dev/media0 -V \"'ov5645 3-003c':0 [fmt:UYVY8_2X8/1280x960 field:none]\"",

    NULL
};

/* Commands to initialize MIPI camera for RZ/G2M/N */
const gchar *rzg2mn_mipi_init_steps[] =
{
    /* Reset all links before enabling new links */
    "media-ctl -d /dev/media0 -r",

    /* Link "VIN4" to "CSI20/VC0". After this, we can start capturing video data from "/dev/video4"
     * which is associated with "VIN4" */
    "media-ctl -d /dev/media0 -l \"'rcar_csi2 fea80000.csi2':1 -> 'VIN4 output':0 [1]\"",
    "media-ctl -d /dev/media0 -V \"'rcar_csi2 fea80000.csi2':1 [fmt:UYVY8_2X8/1280x960 field:none]\"",
    "media-ctl -d /dev/media0 -V \"'ov5645 2-003c':0 [fmt:UYVY8_2X8/1280x960 field:none]\"",

    NULL
};

const gchar *supported_platforms[] = { "ek874", "hihope-rzg2m", "hihope-rzg2n" };

/* List of MIPI camera based on RZ/G2 platforms */
const gchar *mipi_camera_fds[] = { "video0", "video4", "video4" };

/* Warning: "mipi_init_steps" array must have the same size as "supported_platforms" array */
const gchar **mipi_init_steps[] =
{
    /* MIPI initialization steps on RZ/G2E platform */
    rzg2e_mipi_init_steps,

    /* MIPI initialization steps on RZ/G2M platform */
    rzg2mn_mipi_init_steps,

    /* MIPI initialization steps on RZ/G2N platform */
    rzg2mn_mipi_init_steps
};

/* ---------- Private functions ---------- */

gint supported_platform_get_index()
{
    const gint platform_arr_size = (const gint)(sizeof(supported_platforms) / sizeof(const gchar*));
    gint index = 0;

    /* Get hostname */
    const gchar *host_name = g_get_host_name();

    /* Check if hostname exist in the array or not? */
    for (index = 0; index < platform_arr_size; index++)
    {
        /* If it exists, exit the loop immediately and exit the function */
        if (g_strcmp0(supported_platforms[index], host_name) == 0)
        {
            break;
        }
    }

    /* If it does not exist, set index to -1 and exit the function */
    if (index == platform_arr_size)
    {
        index = -1;
    }

    return index;
}

/* ---------- Public functions ---------- */

gboolean mipi_camera_is_supported()
{
    /* Return TRUE if the host name exists in "supported_platforms" array */
    return (supported_platform_get_index() != -1) ? TRUE : FALSE;
}

struct camera_t *mipi_camera_init()
{
    struct camera_t *mipi_camera = NULL;

    gint platform_index = 0;
    gint command_index = 0;

    const gchar *mipi_init_cmd = NULL;
    gint exec_code = 0;

    /* Get index of "supported_platforms" array */
    platform_index = supported_platform_get_index();
    if (platform_index == -1)
    {
        g_critical("Error: MIPI camera is not supported on '%s'", g_get_host_name());
    }
    else
    {
        g_debug("Info: Initializing MIPI camera");

        /* Try to initialize MIPI camera */
        command_index = 0;
        mipi_init_cmd = mipi_init_steps[platform_index][command_index];

        do
        {
            /* Try to run command "mipi_init_cmd" */
            exec_code = system((const char*)mipi_init_cmd);
            if (exec_code != 0)
            {
                /* If failed to run, exit the loop immediately to report error */
                break;
            }

            /* Move to the next command */
            command_index++;
            mipi_init_cmd = mipi_init_steps[platform_index][command_index];
        }
        while (mipi_init_cmd != NULL);

        if (mipi_init_cmd != NULL)
        {
            /* This means that the function cannot run all commands */
            g_debug("Error: Failed to run command '%s'", mipi_init_cmd);
        }
        else
        {
            /* All commands have already run successfully */
            g_message("Info: Initialize MIPI camera successfully");

            mipi_camera = g_new(struct camera_t, 1);

            /* Set required data to "camera_t" object.
             * The order of functions is important */
            camera_set_type(mipi_camera, MIPI_CAMERA);
            camera_set_id(mipi_camera, mipi_camera_fds[platform_index]);
        }
    }

    return mipi_camera;
}

struct camera_t *usb_camera_create(const gchar *camera_fd)
{
    struct camera_t *usb_cam = NULL;

    /* Check parameter(s) */
    g_return_val_if_fail(camera_fd != NULL, NULL);

    /* Create new "camera_t" object */
    usb_cam = g_new(struct camera_t, 1);

    /* Set required data to "camera_t" object.
     * The order of functions is important */
    camera_set_type(usb_cam, USB_CAMERA);
    camera_set_id(usb_cam, camera_fd);

    return usb_cam;
}

struct camera_t *fake_camera_create(const gchar *path)
{
    struct camera_t *fake_cam = NULL;

    /* Check parameter(s) */
    g_return_val_if_fail(path != NULL, NULL);

    /* Note that an absolute path is required for fake cameras to work */
    g_return_val_if_fail(g_path_is_absolute(path), NULL);

    /* Create new "camera_t" object */
    fake_cam = g_new(struct camera_t, 1);

    /* Set required data to "camera_t" object.
     * The order of functions is important */
    camera_set_type(fake_cam, FAKE_CAMERA);
    camera_set_id(fake_cam, path);

    return fake_cam;
}

gboolean usb_camera_is_existed(const gchar *camera_fd)
{
    gchar dev_file[20];
    gchar file_name[50];
    gchar file_content[100];

    /* Check parameter(s) */
    g_return_val_if_fail(camera_fd != NULL, FALSE);

    /* Check the existence of /dev/<camera_fd> (such as: /dev/video8, /dev/video9) */
    g_sprintf(dev_file, "/dev/%s", camera_fd);

    if (!g_file_test(dev_file, G_FILE_TEST_EXISTS))
    {
        /* Return error if /dev/<camera_fd> not found */
        g_debug("Error: Device file '%s' not found", dev_file);
        return FALSE;
    }

    g_debug("Info: Device file '%s' found", dev_file);

    /* Check USB name inside /sys/class/video4linux/<camera_fd>/name */
    g_sprintf(file_name, "/sys/class/video4linux/%s/name", camera_fd);

    FILE *fd = fopen((const char*)file_name, "rt");
    if (fd == NULL)
    {
        return FALSE;
    }

    /* Get the first line of the file
     * Note: This statement always works. No need to check it */
    fgets(file_content, sizeof(file_content), fd);

    /* Try verifying the camera */
    if (g_strstr_len(file_content, sizeof(file_content), "UVC Camera") == NULL)
    {
        g_debug("Info: USB camera '%s' is successfully verified", camera_fd);
    }
    else
    {
        /* Just checking. Some USB camera does not contain "UVC Camera" in it */
        g_message("Warning: Cannot verify USB camera '%s' (optional)", camera_fd);
    }

    /* Close file */
    fclose(fd);

    return TRUE;
}

void camera_print_all(const struct camera_t *camera, gchar *info)
{
    /* Check parameter(s) */
    g_return_if_fail((camera != NULL) && (info != NULL));

    /* Extract camera type */
    g_sprintf(info, "Type: %s; ID: %s", camera_get_type_str(camera), camera_get_id(camera));
}

const gchar* camera_type_to_string(const enum camera_type_t type)
{
    const gchar* result = "";

    switch (type)
    {
        case MIPI_CAMERA:
            result = "MIPI camera";
        break;

        case USB_CAMERA:
            result = "USB camera";
        break;

        case FAKE_CAMERA:
            result = "Fake camera";
        break;

        default:
            result = "Unknown camera";

            /* Raise error message */
            g_critical("Error: Camera type is undefined");
        break;
    }

    return result;
}

const gchar* camera_get_type_str(const struct camera_t *camera)
{
    /* Check parameters */
    g_return_val_if_fail(camera != NULL, "");

    return camera_type_to_string(camera->type);
}

enum camera_type_t camera_get_type(const struct camera_t *camera)
{
     g_return_val_if_fail(camera != NULL, UNKNOWN_CAMERA);

     return camera->type;
}

const gchar* camera_get_id(const struct camera_t *camera)
{
    const gchar* result = "";

    /* Check parameter(s) */
    g_return_val_if_fail(camera != NULL, "");

    switch (camera->type)
    {
        case MIPI_CAMERA:
        case USB_CAMERA:
            result = (camera->id).fd;
        break;

        case FAKE_CAMERA:
            result = (camera->id).file_path;
        break;

        default:
            g_critical("Error: Camera type is undefined");
        break;
    }

    return result;
}

void camera_set_id(struct camera_t *camera, const gchar *id)
{
    /* Check parameter(s) */
    g_return_if_fail((camera != NULL) && (id != NULL));

    /* Set camera ID */
    switch (camera->type)
    {
        case MIPI_CAMERA:
        case USB_CAMERA:
            g_sprintf((camera->id).fd, "/dev/%s", id);
        break;

        case FAKE_CAMERA:
            g_sprintf((camera->id).file_path, "%s", id);
        break;

        default:
            g_critical("Error: Camera type is undefined");
        break;
    }
}

void camera_set_type (struct camera_t *camera, const enum camera_type_t type)
{
    /* Check parameter(s) */
    g_return_if_fail(camera != NULL);

    /* Set camera type */
    camera->type = type;
}
