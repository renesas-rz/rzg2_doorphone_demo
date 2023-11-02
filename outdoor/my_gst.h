/***********************************************************************
 * FILENAME: my_gst.h
 *
 * DESCRIPTION:
 *   Contains APIs related to GStreamer framework.
 *
 * PUBLIC FUNCTIONS:
 *   gboolean gst_get_camera_pipeline(const struct camera_t *camera, gchar *pipeline);
 *
 * AUTHOR: RVC       START DATE: 09/01/2020
 *
 * CHANGES:
 *
 ***********************************************************************/

#ifndef _MY_GST_H_
#define _MY_GST_H_

/* ---------- Macros ---------- */

#define USB_CAM_PIPELINE_FMT_STR_DEFAULT "( v4l2src device=\"%s\" io-mode=dmabuf "               \
                                         "! video/x-raw, format=YUY2, width=1280, height=720 "   \
                                         "! vspmfilter dmabuf-use=true "                         \
                                         "! video/x-raw, format=NV12 "                           \
                                         "! omxh264enc target-bitrate=4000000 quant-p-frames=0 " \
                                         "! video/x-h264, profile=high "                         \
                                         "! h264parse "                                          \
                                         "! video/x-h264, stream-format=avc, alignment=au "      \
                                         "! rtph264pay pt=96 name=pay0 config-interval=3 )"

#define USB_CAM_PIPELINE_FMT_STR "( v4l2src device=\"%s\" "                              \
                                 "! video/x-raw, format=YUY2, width=%s, height=%s "      \
                                 "! videoconvert "                                       \
                                 "! video/x-raw, format=NV12, "                          \
                                 "! omxh264enc target-bitrate=4000000 quant-p-frames=0 " \
                                 "! video/x-h264, profile=high "                         \
                                 "! h264parse "                                          \
                                 "! video/x-h264, stream-format=avc, alignment=au "      \
                                 "! rtph264pay pt=96 name=pay0 config-interval=3 )"

#define MIPI_CAM_PIPELINE_FMT_STR_DEFAULT "( v4l2src device=\"%s\" io-mode=dmabuf-import "               \
                                          "! video/x-raw, format=YUY2, width=1280, height=960, framerate=30/1 "          \
                                          "! vspfilter "                                                 \
                                          "! video/x-raw, format=NV12 "                                  \
                                          "! omxh264enc target-bitrate=4000000 quant-p-frames=0 "        \
                                          "! video/x-h264, profile=high "                                \
                                          "! h264parse ! video/x-h264, stream-format=avc, alignment=au " \
                                          "! rtph264pay pt=96 name=pay0 config-interval=3 )"

#define MIPI_CAM_PIPELINE_FMT_STR "( v4l2src device=\"%s\" io-mode=dmabuf-import "               \
                                  "! video/x-raw, format=YUY2, width=1280, height=960, framerate=30/1 "          \
                                  "! vspfilter "                                                 \
                                  "! video/x-raw, format=NV12, width=%s, height=%s "             \
                                  "! omxh264enc target-bitrate=4000000 quant-p-frames=0 "        \
                                  "! video/x-h264, profile=high "                                \
                                  "! h264parse ! video/x-h264, stream-format=avc, alignment=au " \
                                  "! rtph264pay pt=96 name=pay0 config-interval=3 )"

#define FAKE_CAM_PIPELINE_FMT_STR "( filesrc location=\"%s\" "                            \
                                  "! qtdemux "                                            \
                                  "! h264parse "                                          \
                                  "! video/x-h264, stream-format=avc, alignment=au "      \
                                  "! rtph264pay pt=96 name=pay0 config-interval=3 )"

/* ---------- Functions ---------- */

/*
 * Function: print_supported_resolutions
 * ---
 *   Print supported resolutions of camera on console.
 *
 *   resolution: Pointer to resolution inputted by user.
 *   supported_resolutions: Pointer to list of supported resolutions.
 *
 *   return: void.
 */
void print_supported_resolutions (gchar *resolution, const gchar* supported_resolutions[]);

/*
 * Function: gboolean check_resolution
 * ---
 *   Check resolution inputted by user is supported or not.
 *
 *   resolution: Pointer to resolution inputted by user.
 *   supported_resolutions: Pointer to list of supported resolutions.
 *
 *   return: void.
 */
gboolean check_resolution (gchar *resolution, const gchar *supported_resolutions[]);

/*
 * Function: gst_get_camera_pipeline
 * ---
 *   Creates camera pipeline.
 *
 *   camera: Pointer to "struct camera_t".
 *   pipeline: Pipeline (output).
 *   width: Pointer to width of camera.
 *   height: Pointer to height of camera.
 *
 *   return: TRUE (if successfully create camera pipeline).
 *           FALSE (if unable to get camera pipeline).
 *
 *   Note: This function is not thread-safe.
 */
gboolean gst_get_camera_pipeline(const struct camera_t *camera, gchar *pipeline,
                                 gchar *width, gchar *height);

/*
 * Function: gst_create_urls
 * ---
 *   Create URLs in format "prefix-%d".
 *
 *   count: The number of urls.
 *
 *   return: Array of strings.
 *
 *   Note: The output should be de-allocated if it is not used anymore.
 *
 */
GArray* gst_create_urls(const gchar *prefix, const gint count);

#endif
