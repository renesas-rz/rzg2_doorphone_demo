/***********************************************************************
 * FILENAME: param.h
 *
 * DESCRIPTION:
 *   Contains APIs to parse and access program parameters.
 *
 * PUBLIC FUNCTIONS:
 *   gboolean param_parse(int *argc, char ***argv, gchar *error_str);
 *
 *   void param_free();
 *
 *   void param_print_all();
 *
 *   const gchar* param_get_version();
 *
 *   gboolean param_is_version_enabled();
 *
 *   void param_get_rtsp_server_ports(int **ports, gint *size);
 *
 *   gboolean param_get_cameras(struct camera_t ***cameras, gint *size);
 *
 * AUTHOR: RVC       START DATE: 25/12/2019
 *
 * CHANGES:
 * 
 ***********************************************************************/

#ifndef _PARAM_H_
#define _PARAM_H_

/* ---------- Functions ---------- */

/*
 * Function: param_parse
 * ---
 *   Parse commandline arguments.
 *
 *   If the parsing is successful, any parsed arguments are removed from the array
 *   and argc and argv are updated accordingly. Otherwises, return error inside "error_str".
 *
 *   argc: A pointer to the number of command line arguments.
 *   argv: A pointer to the array of command line arguments.
 *   error_str: Error string if failed to parse.
 *
 *   Note: Must call "param_free()" to de-allocate resources at the end of the program.
 *
 *   returns: TRUE (if params are parsed successfully).
 *            FALSE (if an error occurred when parsing).
 */
gboolean param_parse(int *argc, char ***argv, gchar *error_str);

/*
 * Function: param_free
 * ---
 *   Frees param_t, context, and all the groups which have been added to it.
 *
 *   returns: void.
 */
void param_free();

/*
 * Function: param_print_all
 * ---
 *   Print all information inside "param_t" struct.
 *
 *   returns: void.
 */
void param_print_all();

/*
 * Function: program_get_version
 * ---
 *   Print program version.
 *
 *   Note: The output string must not be modified or deallocated.
 *
 *   returns: gchar* (string containing version).
 */
const gchar* param_get_version();

/*
 * Function: param_is_version_enabled
 * ---
 *   Check if user queries program version or not?
 *
 *   returns: TRUE (program version is queried).
 *            FALSE (program version is not queried).
 */
gboolean param_is_version_enabled();

/*
 * Function: param_get_rtsp_server_ports
 * ---
 *   Get RTSP server's ports.
 *
 *   ports: Port list (output).
 *   size: Port counts (output).
 *
 *   returns: void.
 */
void param_get_rtsp_server_ports(int **ports, gint *size);

/*
 * Function: param_get_cameras
 * ---
 *   Get camera list from "param_t::cameras".
 *
 *   cameras: Camera list (output).
 *   size: Camera size (output)
 *
 *   returns: TRUE ("cameras" array is trusted).
 *            FALSE ("cameras" array cannot gather enough cameras or
 *                   incorrect parameters).
 */
gboolean param_get_cameras(struct camera_t ***cameras, gint *size);

/*
 * Function: param_get_resolution
 * ---
 *   Get width, height from "param_t::width" and "param_t::height".
 *
 *   width: Camera width (output).
 *   height: Camera height (output)
 *
 *   returns: TRUE (Get width and height successfully).
 *            FALSE (incorrect parameters).
 */
gboolean param_get_resolution(gchar *width, gchar *height);
#endif
