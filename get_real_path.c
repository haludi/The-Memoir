int32_t
_get_real_path(char* dir_path, char** real_path, int32_t *detailed_error_code)
{
    const int32_t INIT_SIZE = 128;

    struct stat sb;
    char first;
    int32_t rc;

    int path_length = strlen(dir_path);
    int buf_size = INIT_SIZE / 2 < path_length ? INIT_SIZE * 2 : INIT_SIZE;
    char *link_name = malloc(buf_size);
    char *link_name2 = NULL;

    if (link_name == NULL)
    {
        rc = FAIL_NOMEM;
        goto error_cleanup;
    }
    strcpy(link_name, dir_path);

    int32_t i;
    for (i = 0; i < 256; i++)
    {
        /*int32_t steps = 10;*/

        while (1)
        {
            if (lstat(link_name, &sb) == -1)
            {
                rc = FAIL_STAT_FILE;
                goto error_cleanup;
            }

            int32_t length = readlink(link_name, &first, 1);
            if (length == -1)
            {
                if (errno == EINVAL)
                {
                    /* EINVAL on non-symlink */
                    *real_path = link_name;
                    rc = SUCCESS;
                    goto cleanup;
                }
                printf("6 ");
                rc = FAIL_READ_LINK;
                goto error_cleanup;
            }

            char *current = NULL;
            int32_t required;

            if (first == '/')
            {
                printf("7 ");
                required = sb.st_size + 1;
                current = link_name;
                if (buf_size < required)
                {
                    printf("9 ");
                    buf_size = required + INIT_SIZE;
                    link_name2 = link_name;
                    link_name = malloc(buf_size);
                    if (link_name == NULL)
                    {
                        /*For release*/
                        link_name = link_name2;
                        printf("10 ");
                        rc = FAIL_NOMEM;
                        goto error_cleanup;
                    }

                    free(link_name2);
                }

                length = readlink(link_name, link_name, sb.st_size);
                if (length == -1)
                {
                    printf("11 ");
                    rc = FAIL_READ_LINK;
                    goto error_cleanup;
                }
                printf("12 ");
                path_length = length;
            }
            else
            {
                printf("a-8 ");
                required = path_length + sb.st_size + 1;
                if (buf_size < required)
                {
                    printf("a-9 ");
                    buf_size = required + INIT_SIZE;
                    link_name2 = link_name;
                    link_name = malloc(buf_size);
                    if (link_name == NULL)
                    {
                        /*For release*/
                        link_name = link_name2;
                        printf("a-10 ");
                        rc = FAIL_NOMEM;
                        goto error_cleanup;
                    }

                    strcpy(link_name, link_name2);
                    free(link_name2);
                }

                link_name2 = strdup(link_name);
                if (link_name == NULL)
                {
                    /*For release*/
                    printf("a-10 ");
                    rc = FAIL_NOMEM;
                    goto error_cleanup;
                }
                current = strrchr(link_name, '/');
                length = readlink(link_name2, current + 1, sb.st_size);
                free(link_name2);
                if (path_length == -1)
                {
                    printf("a-11 ");
                    rc = FAIL_READ_LINK;
                    goto error_cleanup;
                }
                printf("a-12 ");
                path_length += length + 1;
            }

            current[path_length] = '\0';
            printf("\nlink_name:%s\n", link_name);
            break;
        }
        printf("\n________________\n");
    }

    rc = FAIL_PATH_RECURSION;

error_cleanup:
    free(link_name);
    *detailed_error_code = errno;
cleanup:
    return rc;
}
