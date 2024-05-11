/* image magic cli : mogrify -format bmp original.jpg */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

struct tinfo {
    int id;
    int width;
    int height;
    int padding;
    FILE * fin;
    FILE * fout;
};

pthread_mutex_t r = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t w = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rw = PTHREAD_MUTEX_INITIALIZER;
int th_num;

/*
 * I read that the fread and fwrite functions, do not move the 
 * file pointers, so there is no need for locks
*/

void * to_gray_thread(void * targs)
{
    struct tinfo * loc_targs = targs;
    int start = loc_targs->id * (loc_targs->height / th_num);
    int end = start + (loc_targs->height / th_num);
    
    if (loc_targs->id == th_num - 1)
        end = loc_targs->height;

    unsigned char pixel[3];
    for (int y = start; y < end; ++y)
    {
        for (int x = 0; x < loc_targs->width; ++x)
        {
            // pthread_mutex_lock(&r);
            fread(pixel, 3, 1, loc_targs->fin);
            // pthread_mutex_unlock(&r);

            unsigned char gray = pixel[0] * 0.299 + pixel[1] * 0.587 + pixel[2] * 0.114;
            memset(pixel, gray, sizeof(pixel));
            
            // pthread_mutex_lock(&w);
            fwrite(&pixel, 3, 1, loc_targs->fout);
            // pthread_mutex_unlock(&w);
        }
        // pthread_mutex_lock(&rw);
        fread(pixel, loc_targs->padding, 1, loc_targs->fin);
        fwrite(pixel, loc_targs->padding, 1, loc_targs->fout);
        // pthread_mutex_unlock(&rw);
    }
}

int main(int argc, char *args[])
{
    if (argc != 2)
    {
        printf("Usage: <%s> <th_num>", args[0]);
        exit(1);
    }

    th_num = strtol(args[1], NULL, 10);

    pthread_t tid[th_num];
    struct tinfo targs[th_num];

    FILE *fIn = fopen("original.bmp", "rb");
    FILE *fOut = fopen("new.bmp", "wb");
    if (!fIn || !fOut)
    {
        printf("File error.\n");
        exit(2);
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, fIn);
    fwrite(header, sizeof(unsigned char), 54, fOut);

    int width = *(int*)&header[18];
    int height = abs(*(int*)&header[22]);
    int stride = (width * 3 + 3) & ~3;
    int padding = stride - width * 3;

    printf("width: %d (%d)\n", width, width * 3);
    printf("height: %d\n", height);
    printf("stride: %d\n", stride);
    printf("padding: %d\n", padding);

    time_t start = time(NULL);
    for (int i = 0; i < th_num; i++)
    {
        targs[i].id = i;
        targs[i].padding = padding;
        targs[i].height = height;
        targs[i].width = width;
        targs[i].fin = fIn;
        targs[i].fout = fOut;
        
        pthread_create(&tid[i], NULL, to_gray_thread, &targs[i]);
    }

    for (int i = 0; i < th_num; ++i)
        pthread_join(tid[i], NULL);
    time_t end = time(NULL);

    printf("Time taken: %d seconds", end - start);

    fclose(fOut);
    fclose(fIn);

    return EXIT_SUCCESS;
}
