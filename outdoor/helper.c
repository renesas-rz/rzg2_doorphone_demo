/***********************************************************************
 * FILENAME: helper.c
 *
 * DESCRIPTION:
 *   Utilitiy function implementations.
 * 
 * NOTE: 
 *   For more further information about function usages, please see file "helper.h".
 * 
 * AUTHOR: RVC       START DATE: 02/08/2017
 *
 * CHANGES:
 * 
 ***********************************************************************/

/* ---------- Header files ---------- */

#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "helper.h"

/* ---------- Functions ---------- */

gboolean kernel_module_is_loaded(const gchar *module_name)
{
    gboolean result = FALSE;

    FILE *fd = NULL;
    gchar line[100];

    /* Check parameter(s) */
    g_return_val_if_fail(module_name != NULL, FALSE);

    /* Try to open PROC_MODULES_PATH to read loaded kernel modules */
    fd = fopen("/proc/modules", "rt");
    if (fd == NULL)
    {
        g_critical("Error: Unable to open file /proc/modules");
    }
    else
    {
        /* Read every line inside the "file" */
        while (fgets((char*)line, sizeof(line), fd) != NULL)
        {
            /* Look for any occurrences of "module_name" inside "line" */
            if (g_strstr_len((const char*)line, sizeof(line), (const char*)module_name) != NULL)
            {
                /* Return positive result if found */
                result = TRUE;
                break;
            }
        }

        /* Close file */
        fclose(fd);
    }

    return result;
}

void error_set(GError **error, const gint err_code, const gchar *format, ...)
{
    gchar err_str[100];

    va_list args;
    va_start(args, format);

    /* Format error message */
    g_vsprintf(err_str, format, args);
    va_end(args);

    /* Set error code and message to GError */
    g_set_error(error, g_quark_from_string(err_str), err_code, "%s", err_str);
}

gboolean file_get(const gchar *dir, const gchar *ext, GArray **arr, gint *size)
{
    gchar *file_name = NULL;
    const gchar *file_ext = NULL;

    gchar *output = NULL;

    /* Check parameter(s) */
    g_return_val_if_fail((dir != NULL) && (ext != NULL) && (arr != NULL) && (size != NULL), FALSE);

    /* Open directory */
    GDir *opened_dir = g_dir_open(dir, 0, NULL);
    if (opened_dir == NULL)
    {
        g_critical("Error: Unable to open directory %s", dir);
    }
    else
    {
        /* Initialize new array */
        *arr = g_array_new(FALSE, FALSE, sizeof(gchar*));
        *size = 0;

        /* Get all file names inside "dir" */
        while ((file_name = (gchar*)g_dir_read_name(opened_dir)) != NULL)
        {
           /* The "file_name" variable should not be modified or freed.
            * Link: https://developer.gnome.org/glib/stable/glib-File-Utilities.html#g-dir-read-name
            */

           /* Extract file extension from "file_name" */
           file_ext = file_get_extension(file_name);           

           /* Filter. Only accept files which contain extension "ext" */
           if (g_ascii_strcasecmp(file_ext, ext) == 0)
           {
               /* Duplicate "file_name" string and store it inside "arr" array  */
               output = g_strdup(file_name);
               g_array_append_val(*arr, output);

               (*size)++;
           }
        }

        /* Raise warning if there are no suitable files */
        if (*size == 0)
        {
            g_debug("Warning: Unable to get '%s' files", ext);
        }
        else
        {
            g_debug("Info: Collected %d '%s' file(s)", *size, ext);
        }

        /* Close directory */
        g_dir_close(opened_dir);
    }

    return FALSE;        
}

void string_array_free(GArray *array, const gint size)
{
    /* Check parameter(s) */
    g_return_if_fail((array != NULL) && (size >= 0));

    gint index = 0;

    for (index = 0; index < size; index++)
    {
        /* Get element from the "array" */
        gchar *item = g_array_index(array, gchar*, index);

        /* Free the element */
        g_free(item);
    }

    /* Free the "array" */
    g_array_free(array, FALSE);
}

const char* file_get_extension(const gchar *file_name)
{
    gchar *file_ext = "";
    gchar *dot_ptr = NULL;

    gchar *temp = NULL;
    gchar *temp_name = NULL;

    /* Check parameter(s) */
    g_return_val_if_fail(file_name != NULL, "");

    /* Ignore if "file_name" is actually a directory */
    if (g_file_test(file_name, G_FILE_TEST_IS_DIR))
    {
        return file_ext;
    }

    /* Remove directory name from "file_name" (if it exists) */
    temp = g_path_get_basename(file_name);
    temp_name = g_strrstr(file_name, temp);

    if (temp_name != NULL)
    {
        dot_ptr = g_strrstr(temp_name, ".");

        if ((dot_ptr != NULL) && (dot_ptr != temp_name))
        {
            /* If result of "g_strrstr" is valid, extract file extension */
            file_ext = dot_ptr + 1;
        }
    }

    /* Free sources */
    g_free(temp);

    return file_ext;
}
