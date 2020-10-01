/***********************************************************************
 * FILENAME: my_gst.c
 *
 * DESCRIPTION:
 *   GStreamer-related function implementations.
 *
 * NOTE:
 *   For more further information about function usages, please see file "my_gst.h".
 *
 * AUTHOR: RVC       START DATE: 02/08/2017
 *
 * CHANGES:
 *
 ***********************************************************************/

/* ---------- Header files ---------- */
#include <glib.h>
#include <glib/gprintf.h>

#include "camera.h"
#include "my_gst.h"

/* ---------- Variables ---------- */
/* Supported resolutions of MIPI camera */
const gchar *mipi_resolutions[] = {
  "640x480",
  "800x600",
  "1280x960",
  NULL,
};


/* Supported resolutions of USB camera */
const gchar *usb_resolutions[] = {
  "640x480",
  "800x600",
  "1280x720",
  NULL,
};

/* ---------- Functions ---------- */

void print_supported_resolutions (gchar *resolution, const gchar* supported_resolutions[]) {
  int index = 0;
  g_message ("%s is unsupported resolution.", resolution);
  g_message ("Please try one of the following resolutions:");

  /* Print list of supported resolutions */
  while (supported_resolutions[index]) {
    g_message ("%s", supported_resolutions[index]);
    index++;
  }
}

gboolean check_resolution (gchar *resolution, const gchar *supported_resolutions[]) {
  gint index = 0;
  gboolean ret = FALSE;

  while (supported_resolutions[index] != NULL) {
    if (g_strcmp0 (supported_resolutions[index], resolution) == 0) {
      ret = TRUE;
      break;
    } else {
      index++;
    }
  }

  if (!ret) {
    print_supported_resolutions (resolution, supported_resolutions);
  }

  return ret;
}

gboolean gst_get_camera_pipeline(const struct camera_t *camera, gchar *pipeline,
                                 gchar *width, gchar *height)
{
    gboolean result = TRUE;
    gchar resolution[20];

    g_return_val_if_fail((camera != NULL) && (pipeline != NULL), FALSE);

    /* Get camera pipeline */
    enum camera_type_t camera_type = camera_get_type(camera);

    switch (camera_type)
    {
        case MIPI_CAMERA:
            if (!g_strcmp0 (width,"")) {
                g_sprintf(pipeline, MIPI_CAM_PIPELINE_FMT_STR_DEFAULT, camera_get_id(camera));

                /* Print debug message */
                g_debug("Info: Pipeline of camera '%s': \"%s\"", camera_get_type_str(camera), pipeline);
            } else {
                sprintf (resolution, "%sx%s", width, height);
                if (check_resolution (resolution, mipi_resolutions)) {
                    g_sprintf(pipeline, MIPI_CAM_PIPELINE_FMT_STR, camera_get_id(camera), width, height);

                    /* Print debug message */
                    g_debug("Camera resolution: %s",resolution);
                    g_debug("Info: Pipeline of camera '%s': \"%s\"", camera_get_type_str(camera), pipeline);
                } else {
                    result = FALSE;
                }
            }

        break;

        case USB_CAMERA:
            if (!g_strcmp0 (width,"")) {
                g_sprintf(pipeline, USB_CAM_PIPELINE_FMT_STR_DEFAULT, camera_get_id(camera));

                /* Print debug message */
                g_debug("Info: Pipeline of camera '%s': \"%s\"", camera_get_type_str(camera), pipeline);
            } else {
                sprintf (resolution, "%sx%s", width, height);
                if (check_resolution (resolution, usb_resolutions)) {
                    g_sprintf(pipeline, USB_CAM_PIPELINE_FMT_STR, camera_get_id(camera), width, height);

                    /* Print debug message */
                    g_debug("Camera resolution: %s",resolution);
                    g_debug("Info: Pipeline of camera '%s': \"%s\"", camera_get_type_str(camera), pipeline);
                } else {
                    result = FALSE;
                }
            }

        break;

        case FAKE_CAMERA:
            g_sprintf(pipeline, FAKE_CAM_PIPELINE_FMT_STR, camera_get_id(camera));

            /* Print debug message */
            g_debug("Info: Pipeline of camera '%s': \"%s\"", camera_get_type_str(camera), pipeline);

        break;

        default:
            g_critical("Error: Cannot get pipeline for camera '%s'", camera_get_type_str(camera));
            result = FALSE;
        break;
    }

    return result;
}

GArray* gst_create_urls(const gchar *prefix, const gint count)
{
    gint index = 0;

    GArray *urls = NULL;
    gchar *url = NULL;

    g_return_val_if_fail((prefix != NULL) && (count >= 0), NULL);

    urls = g_array_new(FALSE, FALSE, sizeof(gchar*));

    for (index = 0; index < count; index++)
    {
        url = g_strdup_printf("/%s-%d", prefix, index + 1);
        g_array_append_val(urls, url);

        g_debug("Info: Created URL '%s'", url);
    }

    return urls;
}
