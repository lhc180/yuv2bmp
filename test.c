#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <WINDOWS.H>
#include <Shell.h>
#include <FileIO.h>
#include "yuv2bmp.h"

void convertEx(IFileIO *fio, const char *file, int width, int height)
{
    char bmpFile[260];
    char suffix[260] = "NV21";
    char *p = NULL;
    FILE *fp = NULL;
    FileIOData *fioData = NULL;
    YUV_FORMAT format = YUV_NV21;
    
    strcpy(bmpFile, file);
    p = strrchr(bmpFile, '.');
    if(p == NULL)
    {
        p = bmpFile;
    }
    else
    {
        strcpy(suffix, p+1);
    }
    strcat(p, ".bmp");
    fp = fopen(bmpFile, "rb");
    if(fp != NULL)
    {
        //already exist, ignore
        fclose(fp);
        return;
    }
    
    p = strrchr(file, '\\');
    if(p != NULL)
    {
        p++;
    }
    else
    {
        p = (char *)file;
    }
    printf("\r                                                                      \rConverting %s...\n", p);        
    
    fioData = ReadFromFile(fio, file);
    if(fioData != NULL)
    {
        strupr(suffix);
        if(strcmp(suffix, "I420")==0)
        {
            format = YUV_I420;
        }
        else if(strcmp(suffix, "YV12")==0)
        {
            format = YUV_YV12;
        }
        else if(strcmp(suffix, "NV12")==0)
        {
            format = YUV_NV12;
        }
        else if(strcmp(suffix, "NV21")==0)
        {
            format = YUV_NV21;
        }

        yuv2bmpfile(format, fioData->buffer, width, height, bmpFile);
        
        FreeData(fio, fioData);
    }
}

void convert(IFileIO *fio, const char *file)
{
    char bmpFile[260];
    char suffix[260] = "NV21";
    int width = 0, height = 0;
    char *p = NULL;
    FILE *fp = NULL;
    FileIOData *fioData = NULL;
    YUV_FORMAT format = YUV_NV21;
    
    strcpy(bmpFile, file);
    p = strrchr(bmpFile, '.');
    if(p == NULL)
    {
        p = bmpFile;
    }
    else
    {
        strcpy(suffix, p+1);
    }
    strcat(p, ".bmp");
    p = strrchr(bmpFile, '_');
    if(p != NULL)
    {
        sscanf(p, "_%dx%d.bmp", &width, &height);
    }
    if(width == 0 || height == 0)
    {
        return;
    }
    fp = fopen(bmpFile, "rb");
    if(fp != NULL)
    {
        //already exist, ignore
        fclose(fp);
        return;
    }
    
    p = strrchr(file, '\\');
    if(p != NULL)
    {
        p++;
    }
    else
    {
        p = (char *)file;
    }
    printf("\r                                                                      \rConverting %s...\n", p);        
    
    fioData = ReadFromFile(fio, file);
    if(fioData != NULL)
    {
        strupr(suffix);
        if(strcmp(suffix, "I420")==0)
        {
            format = YUV_I420;
        }
        else if(strcmp(suffix, "YV12")==0)
        {
            format = YUV_YV12;
        }
        else if(strcmp(suffix, "NV12")==0)
        {
            format = YUV_NV12;
        }
        else if(strcmp(suffix, "NV21")==0)
        {
            format = YUV_NV21;
        }
        
        yuv2bmpfile(format, fioData->buffer, width, height, bmpFile);
        
        FreeData(fio, fioData);
    }
}

BOOL scan_cb(const char *file, void *cbdata)
{
    char suffix[8];
    size_t flen = strlen(file);
    char *p = NULL;
    if(flen < 70)
    {
        p = (char *)file;
    }
    else
    {
        p = (char *)file + flen - 70;
    }
    printf("\r                                                                      \r%s", p);

    p = strrchr(file, (int)'.');
    if(p != NULL)
    {
        strncpy(suffix, p+1, 5);
        suffix[4] = '\0';
        strupr(suffix);
        if(strcmp(suffix, "I420")==0
            || strcmp(suffix, "YV12")==0
            || strcmp(suffix, "NV12")==0
            || strcmp(suffix, "NV21")==0
            )
        {
            convert((IFileIO *)cbdata, file);
        }
    }
    return FALSE;
}

int main(int argc, const char *argv[])
{
    IShell *shell = NULL;
    IFileIO *fio = NULL;
    char *p = NULL;
    
    IShell_Create(&shell);
    IShell_CreateInstance(shell, CID_FILEIO, &fio, 0);
    
    if(argc == 1)
    {
        char dir[260];
        
        HINSTANCE hInst = NULL; 
        GetModuleFileName(hInst, dir, sizeof(dir));
        //strcpy(dir, argv[0]);
        p = strrchr(dir, '\\');
        if(p != NULL)
        {
            *p = '\0';
            ReadDir(fio, dir, NULL, TRUE, scan_cb, fio);
        }
    }
    else if(argc == 2)
    {
        struct _stat st;
        _stat(argv[1], &st);
        if(_S_IFDIR & st.st_mode)
        {
            ReadDir(fio, argv[1], NULL, TRUE, scan_cb, fio);
        }
        else if(_S_IFREG & st.st_mode)
        {
            convert(fio, argv[1]);
        }
    }
    else if(argc == 3)
    {
        int width = 0, height = 0;
        sscanf(argv[2], "%dx%d", &width, &height);
        if(width > 0 && height > 0)
        {
            convertEx(fio, argv[1], width, height);
        }
    }

    Release(fio);
    Release(shell);
    
    return 0;
}

