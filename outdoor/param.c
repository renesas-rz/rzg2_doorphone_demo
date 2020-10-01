/***********************************************************************

 * FILENAME: param.c
 *
 * DESCRIPTION:
 *   Contains APIs to parse and access program parameters.
 * 
 * NOTE: 
 *   For more further information about function usages,
 *   please refer to "param.h".
 * 
 * AUTHOR: RVC       START DATE: 25/12/2019
 *
 * CHANGES:
 * 
 ***********************************************************************/

/* ---------- Header files ---------- */

#include <gst/gst.h>
#include <stdlib.h>
#include <errno.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gmodule.h>
#include <fcntl.h>

#include "camera.h"
#include "param.h"
#include "helper.h"

/* ---------- Macros ---------- */

#define DEFAULT_VIDEO_DIRECTORY "/home/root"
#define DEFAULT_CAMERAS 4

#define DEFAULT_PORT_CAM1 5001
#define DEFAULT_PORT_CAM2 5002
#define DEFAULT_PORT_CAM3 5003
#define DEFAULT_PORT_CAM4 5004

#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151

#define PROGRAM_VERSION "v1.0.0"

#define MP4_VIDEO_EXT "mp4"
#define H264_VIDEO_EXT "h264"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/* ---------- Datatypes ---------- */

/*
 * Struct: param_t
 * ---
 *   Represents program arguments:
 *     - video_dir (string): Location to a directory which contains videos.
 *
 *     - port (array of integers): Contains ports for RTSP servers to listen to.
 *
 *     - version_enabled (gboolean): Set if user would like to know application's version.
 *
 *     - camera_devs (array of "camera_t" objects): Contains information of all cameras.
 *
 *     - usb_cam_fds (array of strings): Contains file descriptor of USB cameras
 *       (should not use while programming).
 *
 *     - usb_cam_arr_size (gint): Represents the number of elements of "param_t::usb_cam_fds" array.
 *
 *     - mipi_cam_enabled (gboolean): Set to TRUE to use MIPI camera.
 *
 *     - video_ext (string): Supported video type (used for fake cameras (camera_dev.h))
 *
 *    - width (string): Set if user would like to change camera resolution
 *
 *    - width (height): Set if user would like to change camera resolution
 */
struct param_t
{
    gchar video_dir[100];

    gint ports[DEFAULT_CAMERAS];

    gboolean version_enabled;

    struct camera_t *cameras[DEFAULT_CAMERAS];

    GArray *usb_cam_fds;

    gint usb_cam_arr_size;

    gboolean mipi_cam_enabled;

    gchar video_ext[20];

    gchar width[10];

    gchar height[10];

};

/* ---------- Private functions ---------- */

/*
 * Function: param_set_video_dir
 * ---
 *   Verifies and sets video directory in "param_t" struct.
 *
 *   For further information related to parameters, please refer to
 *   https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html#GOptionArgFunc
 */
static gboolean param_set_video_dir(const gchar *option_name, const gchar *value,
                                    gpointer data, GError **error);

/*
 * Function: param_set_ports
 * ---
 *   Verifies and sets network ports in "param_t" struct.
 *
 *   For further information related to parameters, please refer to
 *   https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html#GOptionArgFunc
 */
static gboolean param_set_ports(const gchar *option_name, const gchar *value,
                                gpointer data, GError **error);

/*
 * Function: param_set_width
 * ---
 *   Sets width in "param_t" struct.
 *
 *   For further information related to parameters, please refer to
 *   https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html#GOptionArgFunc
 */
static gboolean param_set_width(const gchar *option_name, const gchar *value,
                                gpointer data, GError **error);

/*
 * Function: param_set_height
 * ---
 *   Sets height in "param_t" struct.
 *
 *   For further information related to parameters, please refer to
 *   https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html#GOptionArgFunc
 */
static gboolean param_set_height(const gchar *option_name, const gchar *value,
                                gpointer data, GError **error);

/*
 * Function: param_set_video_ext
 * ---
 *   Verifies and sets supported video extension in "param_t" struct.
 *
 *   For further information related to parameters, please refer to
 *   https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html#GOptionArgFunc
 */
static gboolean param_set_video_ext(const gchar *option_name, const gchar *value,
                                    gpointer data, GError **error);

/*
 * Function: param_enable_mipi_cam
 * ---
 *   Verifies and enables MIPI camera option in "param_t" struct.
 *
 *   For further information related to parameters, please refer to
 *   https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html#GOptionArgFunc
 */
static gboolean param_enable_mipi_cam(const gchar *option_name, const gchar *value,
                                      gpointer data, GError **error);

/*
 * Function: param_set_usb_cam
 * ---
 *   Verifies and sets USB camera's file descriptor in "param_t" struct.
 *
 *   For further information related to parameters, please refer to
 *   https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html#GOptionArgFunc
 */
static gboolean param_set_usb_cam(const gchar *option_name, const gchar *value,
                                  gpointer data, GError **error);

/*
 * Function: camera_array_is_full
 * ---
 *   Check if "param_t::cameras" array is full or not?
 *
 *   returns: TRUE (if the array is full).
 *            FALSE (if the array still has some empty slots).
 *
 *   Note:
 *     - This function is only used for "param_t::cameras".
 */
static gboolean camera_array_is_full();

/*
 * Function: camera_array_add
 * ---
 *   Adds "camera" to "param_t::cameras" array.
 *
 *   camera: Pointer to "camera_t".
 *
 *   returns: TRUE ("camera" is added successfully).
 *            FALSE ("cameras" array is full, "camera" is discarded).
 *
 *   Note:
 *     - This function is only used for "param_t::cameras".
 *     - It will add "camera" if "cameras" array is not full. Otherwises, it discards "camera".
 */
static gboolean camera_array_add(struct camera_t *camera);

/*
 * Function: camera_array_init
 * ---
 *   Initializes "param_t::cameras" array.
 *
 *   returns: void.
 *
 *   Note: This function is only used for "param_t::cameras".
 */
static void camera_array_init();

/*
 * Function: camera_array_init_mipi_cam
 * ---
 *   Initializes MIPI camera for "param_t::cameras" array.
 *
 *   returns: void.
 *
 *   Note: This function is only used for "param_t::cameras" and
 *         is apart of function "camera_array_init".
 */
static void camera_array_init_mipi_cam();

/*
 * Function: camera_array_init_usb_cam
 * ---
 *   Initializes USB cameras for "param_t::cameras" array.
 *
 *   returns: void.
 *
 *   Note: This function is only used for "param_t::cameras" and
 *         is apart of function "camera_array_init".
 */
static void camera_array_init_usb_cam();

/*
 * Function: camera_array_init_fake_cam
 * ---
 *   Initializes fake cameras for "param_t::cameras" array.
 *
 *   returns: void.
 *
 *   Note: This function is only used for "param_t::cameras" and
 *         is apart of function "camera_array_init".
 */
static void camera_array_init_fake_cam();

/* ---------- Variables ---------- */

const gchar *supported_video_exts[] = { MP4_VIDEO_EXT, H264_VIDEO_EXT };

struct param_t param =
{
    .video_dir = DEFAULT_VIDEO_DIRECTORY,

    .ports = { DEFAULT_PORT_CAM1, DEFAULT_PORT_CAM2, DEFAULT_PORT_CAM3, DEFAULT_PORT_CAM4 },

    .cameras = { NULL, NULL, NULL, NULL },

    .usb_cam_fds = NULL,

    .usb_cam_arr_size = 0,

    .mipi_cam_enabled = FALSE,

    .video_ext = MP4_VIDEO_EXT,

    .version_enabled = FALSE,

    .width[0] = '\0',

    .height[0] = '\0',
};

GOptionContext *context = NULL;

GOptionEntry entries[] =
{
    { "video-dir", 'd', G_OPTION_FLAG_NONE, G_OPTION_ARG_CALLBACK, param_set_video_dir,
      "Set an absolute path to video directory", DEFAULT_VIDEO_DIRECTORY },

    { "rtsp-port", 'p', G_OPTION_FLAG_NONE, G_OPTION_ARG_CALLBACK, param_set_ports,
      "Set RTSP server's port", STR(DEFAULT_PORT_CAM1) },

    { "usb-cam", 'u', G_OPTION_FLAG_NONE, G_OPTION_ARG_CALLBACK, param_set_usb_cam,
      "Add USB camera", "video8" },

    { "mipi-cam", 'm', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, param_enable_mipi_cam,
      "Use MIPI camera", NULL },

    { "video-ext", 'e', G_OPTION_FLAG_NONE, G_OPTION_ARG_CALLBACK, param_set_video_ext,
      "Set video extension", MP4_VIDEO_EXT },

    { "version", 'v', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &param.version_enabled,
      "Check version", NULL },

    { "width", 0, G_OPTION_FLAG_NONE, G_OPTION_ARG_CALLBACK, param_set_width,
      "Set camera width", NULL },

    { "height", 0, G_OPTION_FLAG_NONE, G_OPTION_ARG_CALLBACK, param_set_height,
      "Set camera height", NULL },

    { NULL }
};

/* ---------- Private functions ---------- */

gboolean param_set_usb_cam(const gchar *option_name, const gchar *value,
                           gpointer data, GError **error)
{
    gchar *temp = NULL;

    /* Get USB camera's file descriptor */
    const gchar *usb_cam_fd = value;

    /* Check if UVC kernel module is loaded or not? */
    #if 0
    if (!kernel_module_is_loaded("uvcvideo"))
    {
        error_set(error, ENODEV, "Kernel module 'uvcvideo' is not loaded");

        return FALSE;
    }
    #endif

    /* Check if USB camera exists or not? */
    if (!usb_camera_is_existed(usb_cam_fd))
    {
        g_debug("Error: USB camera '%s' does not exist", usb_cam_fd);
        error_set(error, ENODEV, "%s (%s %s)", g_strerror(ENODEV), option_name, value);

        return FALSE;
    }

    /* The USB camera "usb_cam_fd" is valid */
    g_debug("Info: USB camera '%s' is valid", usb_cam_fd);

    /* Allocate new array for the first time */
    if (param.usb_cam_fds == NULL)
    {
        param.usb_cam_fds = g_array_new(FALSE, FALSE, sizeof(gchar*));
        param.usb_cam_arr_size = 0;
    }

    /* Add "usb_cam_fd" to "param_t::usb_cam_fds" array */
    temp = g_strdup(usb_cam_fd);
    g_array_append_val(param.usb_cam_fds, temp);

    param.usb_cam_arr_size++;

    return TRUE;
}

gboolean param_enable_mipi_cam(const gchar *option_name, const gchar *value,
                               gpointer data, GError **error)
{
    gboolean result = TRUE; 

    /* Check if MIPI camera is supported? */
    if (mipi_camera_is_supported())
    {
        g_debug("Info: MIPI camera is supported on '%s'", g_get_host_name());

        /*  If supported, enable MIPI camera option */
        param.mipi_cam_enabled = TRUE;
    }
    else
    {
        /* Set error messages */
        g_debug("Error: MIPI camera is not supported on '%s'", g_get_host_name());
        error_set(error, ENODEV, "%s (%s)", g_strerror(ENODEV), option_name);

        /* If not supported, disable MIPI camera option */
        param.mipi_cam_enabled = FALSE;

        result = FALSE;
    }

    return result;
}

gboolean param_set_video_ext(const gchar *option_name, const gchar *value,
                             gpointer data, GError **error)
{
    gboolean result = TRUE;

    const gint ext_array_size = (const gint)(sizeof(supported_video_exts) / sizeof(gchar*));
    gint index = 0;

    /* Extract video extension */
    const gchar *video_ext = (const gchar*)value;

    /* Verify video extension */
    for (index = 0; index < ext_array_size; index++)
    {
        if (g_ascii_strcasecmp(supported_video_exts[index], video_ext) == 0)
        {
            /* Video extension "video_ext" is supported */
            g_debug("Info: Video extension '%s' is supported", video_ext);

            /* Apply new supported video extension to "param_t::video_ext" */
            g_stpcpy(param.video_ext, supported_video_exts[index]);

            break;
        }
    }

    /* Video extension "video_ext" is not supported */
    if (index == ext_array_size)
    {
        /* Set error messages */
        g_debug("Error: Video extension '%s' is not supported", video_ext);
        error_set(error, EINVAL, "%s (%s %s)", g_strerror(EINVAL), option_name, value);

        result = FALSE;
    }

    return result;
}

gboolean param_set_ports(const gchar *option_name, const gchar *value,
                         gpointer data, GError **error)
{
    gboolean result = TRUE;
    gint index = 0;

    /* Extract network port */
    gint64 origin_port = g_ascii_strtoll(value, NULL, 10);

    if ((origin_port < REGISTERED_PORT_MIN) || (origin_port > REGISTERED_PORT_MAX))
    {
        /* If it is not valid, set error messages */
        g_debug("Error: Failed to parse RTSP server's port (%s %s)", option_name, value);
        error_set(error, EINVAL, "%s (%s %s)", g_strerror(EINVAL), option_name, value);

        result = FALSE;
    }
    else
    {
        g_debug("Info: The RTSP server's port: %d", (gint)origin_port);

        /* If it is valid, assign the values to "param_t::ports" variable */
        for (index = 0; index < DEFAULT_CAMERAS; index++)
        {
            param.ports[index] = (gint)(origin_port + index);
        }
    }

    return result;
}

gboolean param_set_video_dir(const gchar *option_name, const gchar *value,
                             gpointer data, GError **error)
{
    /* Extract video directory */
    const gchar *video_dir = value;

    /* Check if directory exists or not? */
    if (!g_file_test(video_dir, G_FILE_TEST_IS_DIR))
    {
        /* If it does not exist, set error messages */
        g_debug("Error: Directory '%s' does not exist", video_dir);
        error_set(error, ENOENT, "%s (%s %s)", g_strerror(ENOENT), option_name, video_dir);

        return FALSE;
    }

    /* Check if directory's path is absolute path */
    if (!g_path_is_absolute(video_dir))
    {
       /* If it is not an absoluate path, set error messages */
       error_set(error, EINVAL, "'%s' is not an absolute path", video_dir);

       return FALSE;
    }

    g_debug("Info: Directory '%s' is valid", video_dir);

    /* If it is valid, set "video_dir" to "param_t::video_dir" variable */
    g_stpcpy(param.video_dir, video_dir);

    return TRUE;
}

gboolean param_set_width(const gchar *option_name, const gchar *value,
                         gpointer data, GError **error)
{
    const gchar *width = value;
    g_stpcpy(param.width, width);
    g_debug("Camera width: %s", param.width);
    return TRUE;
}

gboolean param_set_height(const gchar *option_name, const gchar *value,
                         gpointer data, GError **error)
{
    const gchar *height = value;
    g_stpcpy(param.height, height);
    g_debug("Camera height: %s", param.height);
    return TRUE;
}

gboolean camera_array_is_full()
{
    gint index = 0;

    /* Try to find an available slot in "param_t::cameras" array */
    for (index = 0; index < DEFAULT_CAMERAS; index++)
    {
        /* If found, the array is not full */
        if (param.cameras[index] == NULL)
        {
            return FALSE;
        }
    }

    return TRUE;
}

gboolean camera_array_add(struct camera_t *camera)
{
    gboolean result = TRUE;
    gint index = 0;

    /* Check parameter(s) */
    g_return_val_if_fail(camera != NULL, FALSE);

    /* Look for an available slot. If found, add "camera" to "param_t::cameras" */
    for (index = 0; index < DEFAULT_CAMERAS; index++)
    {
        if (param.cameras[index] != NULL)
        {
            g_debug("Info: param_t::cameras[%d] is occupied", index);
        }
        else
        {
            g_debug("Info: param_t::cameras[%d] is empty", index);

            param.cameras[index] = camera;
            break;
        }
    }

    /* Raise warning if the array is full */
    if (index == DEFAULT_CAMERAS)
    {
        g_debug("Warning: 'param_t::cameras' array is full");
        result = FALSE;
    }

    return result;
}

void camera_array_init()
{
    /* Note: Do not change the order of the following functions */

    /* Initialize MIPI camera */
    camera_array_init_mipi_cam();

    /* Initialize USB camera(s) */
    camera_array_init_usb_cam();

    /* Initialize fake camera(s) */
    camera_array_init_fake_cam();
}

void camera_array_init_fake_cam()
{
    gint index = 0;

    GArray *file_arr = NULL;
    gint file_arr_size = 0;

    gchar *file_name = NULL;
    gchar *file_path = NULL;

    if (camera_array_is_full())
    {
        g_message("Info: Unable to add fake cameras");
    }
    else
    {
        /* Get files which have supported extension */
        file_get(param.video_dir, param.video_ext, &file_arr, &file_arr_size); 

        for (index = 0; index < file_arr_size; index++)
        {
            /* Create an absolute path to the file "file_name" */
            file_name = g_array_index(file_arr, gchar*, index);
            file_path = g_strdup_printf("%s/%s", param.video_dir, file_name);

            g_debug("Info: Trying to add fake camera '%s'", file_path);
 
            /* Create "camera_t" object from the path */
            struct camera_t *fake_cam = fake_camera_create(file_path);

            /* Free "file_path" */
            g_free(file_path);

            /* Try to add it to the "param_t::cameras" array */
            if (camera_array_add(fake_cam))
            {
                g_debug("Info: Fake camera '%s' added", file_name);
            }
            else
            {
                /* Free up "camera_t" object */
                g_free(fake_cam);

                /* Set error message */
                g_debug("Info: Cannot add fake camera '%s'", file_name);
                break;
            }
        }

        if (file_arr != NULL)
        {
            /* Free "file_arr" array */
            string_array_free(file_arr, file_arr_size);
        }
    }
}

void camera_array_init_usb_cam()
{
    gint index = 0;

    /* Check if user has inputed USB camera(s) */
    if (param.usb_cam_fds == NULL)
    {
        g_debug("Info: USB cameras are not set");
    }
    else
    {
        for (index = 0; index < param.usb_cam_arr_size; index++)
        {
            /* Get USB camera's file descriptor */
            gchar *usb_cam_fd = g_array_index(param.usb_cam_fds, gchar*, index);

            /* Create new "camera_t" object */
            struct camera_t *usb_cam = usb_camera_create(usb_cam_fd);
            if (camera_array_add(usb_cam))
            {
                g_message("Info: USB camera '%s' added", usb_cam_fd);
            }
            else
            {
                /* Free up "camera_t" object */
                g_free(usb_cam);

                /* Raise error message */
                g_message("Error: Cannot add USB camera '%s'", usb_cam_fd);
                break;
            }
        }
    }
}

void camera_array_init_mipi_cam()
{
    /* Add MIPI camera */
    if (!param.mipi_cam_enabled)
    {
        g_debug("Info: MIPI camera option is not enabled");
    }
    else
    {
        g_debug("Info: MIPI camera option is enabled");

        struct camera_t *mipi_camera = mipi_camera_init();
        if (mipi_camera != NULL)
        {
            g_debug("Info: Added MIPI camera");
            camera_array_add(mipi_camera);
        }
        else
        {
            /* Raise error messages */
            g_message("Error: Failed to initialize MIPI camera");
            g_message("Info: Disable MIPI camera option");

            /* Disable MIPI camera option */
            param.mipi_cam_enabled = FALSE;
        }
    }
}

/* ---------- Public functions ---------- */

gboolean param_parse(int *argc, char ***argv, gchar *error_str)
{
    gboolean result = TRUE;
    GError *error = NULL;

    g_return_val_if_fail((argc != NULL) && (argv != NULL) && (error_str != NULL), FALSE);

    context = g_option_context_new ("- send camera and audio streams");
    g_option_context_add_main_entries (context, entries, NULL);
    g_option_context_add_group (context, gst_init_get_option_group());

    if (g_option_context_parse (context, argc, argv, &error))
    {
        /* Initialize "param_t::cameras" if parsing process has no problems.
         *
         * Note that if error happens in this process, the app will automatically
         * disable affected features as much as possible in order to run normally.
         *
         * For example: If MIPI camera is supported, but unable to be initialized
         * successfully, the app will disable this feature and switch to use either
         * USB cameras or fake cameras (using videos).
         */
        camera_array_init();

        /* Check if the app collects enough cameras */
        if (!camera_array_is_full())
        {
            /* Raise error if there are not enough cameras */
            g_stpcpy(error_str, "Not collect enough cameras");

            result = FALSE;
        }
    }
    else
    {
        /* Return error message if unable to parse */
        g_stpcpy(error_str, error->message);

        /* Free resources */
        g_clear_error(&error);

        result = FALSE;
    }

    return result;
}

void param_free()
{
    gint index = 0;

    /* Free option context */
    g_option_context_free(context);

    /* Free "param_t::cameras" */
    for (index = 0; index < DEFAULT_CAMERAS; index++)
    {
        if (param.cameras[index] != NULL)
        {
            g_free(param.cameras[index]);
        }
    }

    /* Free "param_t::usb_cam_fds" */
    if (param.usb_cam_fds != NULL)
    {
        string_array_free(param.usb_cam_fds, param.usb_cam_arr_size);
    }
}

void param_print_all()
{
    gint index = 0;
    gchar camera_info[100];

    /* Print video directory */
    g_message("Video directory: %s", param.video_dir);

    /* Print RTSP ports */
    for (index = 0; index < DEFAULT_CAMERAS; index++)
    {
        g_message("RTSP server's port %d: %d", index + 1, param.ports[index]);
    }

    /* Print cameras */
    for (index = 0; index < DEFAULT_CAMERAS; index++)
    {
        if (param.cameras[index] != NULL)
        {
            camera_print_all(param.cameras[index], camera_info);
            g_message("Camera %d: %s", index + 1, camera_info);
        }
    }

    /* Print MIPI camera support status */
    g_message("Use MIPI camera: %s", (param.mipi_cam_enabled) ? "yes" : "no");

    /* Print supported video extension */
    g_message("Supported video extension: %s", param.video_ext);
}

const gchar* param_get_version()
{
    return PROGRAM_VERSION;
}

gboolean param_is_version_enabled()
{
    return param.version_enabled;
}

void param_get_rtsp_server_ports(int **ports, gint *size)
{
    g_return_if_fail((ports != NULL) && (size != NULL));

    *ports = param.ports;
    *size = DEFAULT_CAMERAS;
}

gboolean param_get_cameras(struct camera_t ***cameras, gint *size)
{
    gboolean result = TRUE;

    g_return_val_if_fail((cameras != NULL) && (size != NULL), FALSE);

    *cameras = param.cameras;
    *size = DEFAULT_CAMERAS;

    if (!camera_array_is_full())
    {
        g_debug("Error: Cannot collect enough cameras");
        result = FALSE;
    }

    return result;
}

gboolean param_get_resolution(gchar *width, gchar *height)
{
    gboolean result = TRUE;

    g_return_val_if_fail((width != NULL) && (height != NULL), FALSE);
    g_stpcpy (width, param.width);
    g_stpcpy (height, param.height);
    //width = g_strdup_printf("%s", param.width);
    //height = g_strdup_printf("%s", param.height);

    return result;
}