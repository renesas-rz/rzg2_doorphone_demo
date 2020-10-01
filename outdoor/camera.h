/***********************************************************************
 * FILENAME: camera.h
 *
 * DESCRIPTION:
 *   Contains APIs to detect and initialize (MIPI, USB) cameras.
 *
 * PUBLIC FUNCTIONS:
 *   gboolean mipi_camera_is_supported();
 *
 *   struct camera_t *mipi_camera_init();
 *
 *   struct camera_t *usb_camera_create(const gchar *camera_fd);
 *
 *   gboolean usb_camera_is_existed(const gchar *camera_fd);
 *
 *   const gchar* camera_type_to_string(const enum camera_type_t type);
 *
 *   const gchar* camera_get_type_str(const struct camera_t *camera);
 *
 *   enum camera_type_t camera_get_type(const struct camera_t *camera);
 *
 *   const gchar* camera_get_id(const struct camera_t *camera);
 *
 *   void camera_set_id(struct camera_t *camera, const gchar *id);
 *
 *   void camera_set_type(struct camera_t *camera, const enum camera_type_t type);
 *
 * AUTHOR: RVC       START DATE: 30/12/2019
 *
 * CHANGES:
 *
 ***********************************************************************/

#ifndef _CAMERA_H_
#define _CAMERA_H_

/* ---------- Datatypes ---------- */

/*
 * Enum: camera_type_t
 * ---
 *   Represents camera type:
 *     - MIPI_CAMERA: Indicate that the camera is MIPI camera.
 *     - USB_CAMERA: Indicate that the camera is USB camera.
 *     - FAKE_CAMERA: Indicate that the camera is actually just a video.
 *     - UNKNOWN_CAMERA: Indicate that the camera is invalid.
 */
enum camera_type_t
{
    MIPI_CAMERA,
    USB_CAMERA,
    FAKE_CAMERA,
    UNKNOWN_CAMERA
};

/*
 * Union: camera_id_t
 * ---
 *   Represents camera ID:
 *     - camera_id_t::fd (string): Store virtual device (such as /dev/video8, /dev/video9...)
 *       if the camera is either MIPI_CAMERA or USB_CAMERA.
 *
 *     - camera_id_t::file_path (string): Store the location to a video
 *       if the camera is FAKE_CAMERA. The search path depends on
 *       prog_params_t::video_directory (param_parser.h).
 */
union camera_id_t;

/*
 * Struct: camera_t
 * ---
 *   Represents camera device:
 *     - type (enum camera_type_t): Camera type (such as: USB, MIPI or fake camera).
 *     - id (union camera_id_t): Camera ID (store either virtual device or video's (absolute) path).
 */
struct camera_t;

/* ---------- Functions ---------- */

/*
 * Function: mipi_camera_is_supported
 * ---
 *   Checks if MIPI camera is supported on this platform or not?
 * 
 *   return: TRUE (MIPI cam is supported).
 *           FALSE (MIPI cam is not supported).
 */
gboolean mipi_camera_is_supported();

/*
 * Function: mipi_camera_init
 * ---
 *   Initializes MIPI camera.
 *
 *   return: NULL (unable to initialize MIPI camera).
 *           not NULL (successfully initialize MIPI camera).
 *
 *   Note: The "camera_t" ouput is allocated dynamically.
 *         Should use "free()" to deallocate if it is not used anymore.
 */
struct camera_t *mipi_camera_init();

/*
 * Function: usb_camera_create
 * ---
 *   Creates USB camera from file descriptor.
 *
 *   camera_fd: File descriptor (such as: video8, video9...).
 *
 *   return: NULL (unable to initialize USB camera).
 *           not NULL (successfully initialize USB camera).
 *
 *   Note: The "camera_t" ouput is allocated dynamically.
 *         Should use "free()" to deallocate if it is not used anymore.
 */
struct camera_t *usb_camera_create(const gchar *camera_fd);

/*
 * Function: fake_camera_create
 * ---
 *   Creates fake camera from video's path.
 *
 *   path: video's path.
 *
 *   return: NULL (unable to initialize fake camera).
 *           not NULL (successfully initialize fake camera).
 *
 *   Note: The "camera_t" ouput is allocated dynamically.
 *         Should use "free()" to deallocate if it is not used anymore.
 */
struct camera_t *fake_camera_create(const gchar *path);

/*
 * Function: usb_camera_is_existed
 * ---
 *   Check if the USB camera exists in the system or not?
 *     - Check the existence of /dev/<camera_fd> (such as: /dev/video8, /dev/video9).
 *
 *     - Check USB name inside /sys/class/video4linux/<camera_fd>/name.
 *       It should display something like this: "UVC Camera (046d:0825)".
 *       Note that this checking is *optional*. It's because some cameras
 *       (such as: Logitech Brio Ultra HD Pro Webcam) displays
 *       its real name instead of "UVC Camera".
 *
 *   camera_fd: File descriptor (such as: video8, video9...).
 *
 *   return: FALSE (the USB camera does not exist).
 *           TRUE (the USB camera exists).
 */
gboolean usb_camera_is_existed(const gchar *camera_fd);

/*
 * Function: camera_print_all
 * ---
 *   Print all information inside "camera_t" struct.
 *
 *   camera: Reference to "camera_t" struct.
 *   info: String which contains the information.
 *
 *   return: void.
 */
void camera_print_all(const struct camera_t *camera, gchar *info);

/*
 * Function: camera_type_to_string
 * ---
 *   Convert "struct camera_type_t" to string.
 *
 *   type: Camera type.
 *
 *   Note: The output string must not be de-allocated or modified.
 *
 *   return: String (camera type).
 */
const gchar* camera_type_to_string(const enum camera_type_t type);

/*
 * Function: camera_get_type_str
 * ---
 *   Get camera type inside "camera_t" struct.
 *
 *   camera: Reference to "camera_t" struct.
 *
 *   Note: The output string must not be de-allocated or modified.
 *
 *   return: String (camera type).
 */
const gchar* camera_get_type_str(const struct camera_t *camera);

/*
 * Function: camera_get_type
 * ---
 *   Get camera type ("camera_type_t") inside "camera_t" struct.
 *
 *   camera: Reference to "camera_t" struct.
 *
 *   return: "enum camera_type_t" (camera type).
 */
enum camera_type_t camera_get_type(const struct camera_t *camera);

/*
 * Function: camera_get_id
 * ---
 *   Get camera ID inside "camera_t" struct.
 *
 *   camera: Reference to "camera_t" struct.
 *
 *   Note: The output string must not be de-allocated or modified.
 *
 *   return: String (camera ID).
 */
const gchar* camera_get_id(const struct camera_t *camera);

/*
 * Function: camera_set_id
 * ---
 *   Set camera "id" to "camera" object.
 *
 *   camera: Reference to "camera_t" struct.
 *   id: Camera ID
 *     - If "camera" is either USB or MIPI camera, the ID would be file descriptor (such as: video8, video9...).
 *     - If "camera" is fake camera, the ID would be the video's path.
 *
 *   return: void.
 */
void camera_set_id(struct camera_t *camera, const gchar *id);

/*
 * Function: camera_set_type
 * ---
 *   Set camera "type" to "camera" object.
 *
 *   camera: Reference to "camera_t" struct.
 *   type: Camera type.
 *
 *   return: void.
 */
void camera_set_type(struct camera_t *camera, const enum camera_type_t type);

#endif
