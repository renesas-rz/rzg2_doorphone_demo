/***********************************************************************
 * FILENAME: helper.h
 *
 * DESCRIPTION:
 *   Utility functions.
 *
 * PUBLIC FUNCTIONS:
 *   gboolean kernel_module_is_loaded(const gchar *module_name);
 *
 *   void error_set(GError **error, const gint err_code, const gchar *format, ...);
 *
 *   gboolean file_get(const gchar *dir, const gchar *ext, GArray **arr, gint *size);   
 *
 *   void string_array_free(GArray *array, const gint size);
 *
 *   const char* file_get_extension(const gchar *file_name);
 *
 * AUTHOR: RVC       START DATE: 03/01/2020
 *
 * CHANGES:
 * 
 ***********************************************************************/

#ifndef _HELPER_H_
#define _HELPER_H_

/* ---------- Functions ---------- */

/*
 * Function: kernel_module_is_loaded
 * ---
 *   Check if a kernel module is loaded or not?
 *
 *   return: TRUE (The kernel module is loaded).
 *           FALSE (The kernel module is not loaded).
 */
gboolean kernel_module_is_loaded(const gchar *module_name);

/*
 * Function: error_set
 * ---
 *   Set error code and message for GError.
 *
 *   error: Pointer to GError*
 *   err_code: Error code (extracted from "errno").
 *   format: Formatted error description.
 *
 *   Note: Be careful! Because it contains complex data types,
 *         there will be no statements for checking parameters.
 *
 *   return: void.
 */
void error_set(GError **error, const gint err_code, const gchar *format, ...);

/*
 * Function: file_get
 * ---
 *   Get files which have extension "ext" inside directory "dir".
 *
 *   dir: Target directory.
 *   ext: File extension, such as: "mp4", "h264"... (not contain dot character).
 *   arr: Array of strings (output).
 *   size: The number of elements inside "arr" array (output).
 *
 *   Note: "arr" is dynamically allocated. Please dispose if it is not used anymore.
 *
 *   return: TRUE (if files are collected successfully).
 *           FALSE (if something's wrong during the process, such as: "dir" not found...)
 */
gboolean file_get(const gchar *dir, const gchar *ext, GArray **arr, gint *size);

/*
 * Function: string_array_free
 * ---
 *   Free "array" which contains dynamically allocated string elements.
 *
 *   array: String array.
 *   size: The number of elements of "array" array.
 *
 *   Note: This function not only de-allocates string elements inside "array",
 *         but also frees up the array itself.
 *
 *   return: void.
 */
void string_array_free(GArray *array, const gint size);

/*
 * Function: file_get_extension
 * ---
 *   Get file extension from "file_name".
 *
 *   file_name: File name (allowed to contain path).
 *
 *   Note: The output string must not be modified or freed.
 *
 *   return: String (file extension) (not contain dot character).
 */
const char* file_get_extension(const gchar *file_name);

#endif
