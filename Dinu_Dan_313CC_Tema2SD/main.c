#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define max(a,b) ((a>b)? a:b)


typedef struct QuadtreeNode //struct for vector 
{
    unsigned char blue, green, red;
    uint32_t area;
    int32_t top_left, top_right;
    int32_t bottom_left, bottom_right;
}__attribute__((packed)) QuadtreeNode;


typedef struct Pixel
{
    unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
}Pixel;

typedef struct Quadtree //struct for Quadtree
{
    Pixel Pixels;
    struct Quadtree *Top_left, *Top_right;
    struct Quadtree *Bottom_left, *Bottom_right;
    long long position;
    long long nr;
    uint32_t Area;
} Quadtree;

 
void Parse_Image(Pixel **Img_Matrix, Quadtree **Node, int factor, int x, int y, int latura) 
{
    int i,j;
    unsigned long long int red = 0, green = 0, blue = 0;
    unsigned long long int Arie = latura * latura;
    unsigned long long int mean = 0;
    
    //aflam culoarea medie pentru cadranul curent

    for(i=y; i<y+latura; i++)
        for(j=x; j<x+latura; j++)
            {
                red += Img_Matrix[i][j].Red;
                green += Img_Matrix[i][j].Green;
                blue += Img_Matrix[i][j].Blue;
            }
 
    blue /= Arie;
    green /= Arie;
    red /= Arie;
 
    //aflam valoarea "mean" pentru cadranul curent

    for(i=y; i<y+latura; i++)
        for(j=x; j<x+latura; j++)
            mean += (red - Img_Matrix[i][j].Red) * (red - Img_Matrix[i][j].Red) + (green - Img_Matrix[i][j].Green) * (green - Img_Matrix[i][j].Green) + (blue - Img_Matrix[i][j].Blue) * (blue - Img_Matrix[i][j].Blue);
    
    mean = mean / (3*Arie);

    //introducem valorile gasite in arbore
    (*Node) = malloc(sizeof(Quadtree));
    (*Node)->Area = latura * latura;

    (*Node)->Pixels.Blue = blue;
    (*Node)->Pixels.Green = green;
    (*Node)->Pixels.Red = red;
 
    //printf("Red: %d\n",(*Node)->Pixels.Red);
    //printf("Blue: %d\n",(*Node)->Pixels.Blue);
    //printf("Green: %d\n",(*Node)->Pixels.Green);
    //printf("Mean: %lld\n",mean);

    if(mean <= factor)
        {
            //daca avem culoare uniforma, acest nod nu va avea fii
            (*Node)->Bottom_left = NULL;
            (*Node)->Bottom_right = NULL;
            (*Node)->Top_left = NULL;
            (*Node)->Top_right = NULL;

        }
    else
        {
            //altfel continuam cu fii nodului curent si coordonatele si dimensiunea subimaginii
            Parse_Image(Img_Matrix, &(*Node)->Top_left, factor, x, y, latura/2);
            Parse_Image(Img_Matrix, &(*Node)->Top_right, factor, x+(latura/2), y, latura/2);
            Parse_Image(Img_Matrix, &(*Node)->Bottom_right, factor, x+(latura/2), y+(latura/2), latura/2);
            Parse_Image(Img_Matrix, &(*Node)->Bottom_left, factor, x, y+(latura/2), latura/2);
        }
}

typedef struct QNode 
{
    Quadtree **data;
    struct QNode *next;
    struct QNode *prev;
} QNode_t;

typedef struct queue 
{
    QNode_t *head;
    QNode_t *tail;
    size_t len;
} queue_t;


QNode_t *initQueueNode(Quadtree **data) {
    QNode_t *newNode = malloc(sizeof(QNode_t));
    newNode->data = data;
    newNode->prev = newNode->next = NULL;

    return newNode;
}

queue_t *initQtypueue() 
{
    queue_t *newQueue = malloc(sizeof(queue_t));
    newQueue->head = newQueue->tail = NULL;
    newQueue->len = 0;

    return newQueue;
}

void enqueue(queue_t *queue, Quadtree **data) 
{
    QNode_t *new = initQueueNode(data);

    if (queue->len == 0) 
    {
        // Coada goala
        queue->head = new;
        queue->tail = new;
    } else {
        // Introducere la inceput de coada
        new->next = queue->head;
        queue->head->prev = new;
        queue->head = new;
    }
    ++queue->len;
}

Quadtree **dequeue(queue_t **queue) {
    Quadtree **ret;

    if ((*queue)->len == 0) {
        // Coada este goala
        return NULL;
    }
    // Elimina nod de la final
    QNode_t *aux = (*queue)->tail;

    if ((*queue)->len == 1) {
        // Coada are un singur element
        (*queue)->tail = NULL;
        (*queue)->head = NULL;
    } else {
        // Coada are mai multe elemente
        (*queue)->tail = aux->prev;
        (*queue)->tail->next = NULL;
    }
    --(*queue)->len;

    // Copiez valoarea nodului
    ret = aux->data;

    // Setez statusul pe 0

    free(aux);
    return ret;
}

int count(Quadtree **Node)//aflam numarul de noduri
{

    /*if(!(*Node)->Top_left)
        {
            //printf("DA");
            return max((*Node)->position,nr);
        }
    else
        {
            return max(max(max(count(&(*Node)->Top_left,nr),count(&(*Node)->Top_right,nr)),count(&(*Node)->Bottom_left,nr)),count(&(*Node)->Bottom_right,nr));                                           
        }
    return 0;*/

    if((*Node) == NULL)
        return 0;
    return 1+count(&(*Node)->Top_left) + count(&(*Node)->Top_right) + count(&(*Node)->Bottom_left) + count(&(*Node)->Bottom_right);

}

void indexing(queue_t *Queue, Quadtree **Node, int *index)
{
    enqueue(Queue,Node);
    while( Queue->len > 0)//cat timp inca avem elemente in coada
        {
            Quadtree **Aux = dequeue(&Queue);//eliminam elementul din coada
            (*Aux)->position = *index;//il indexam
            (*index)++;//incrementam indexul
            if((*Aux)->Top_right != NULL) //adaugam fii in coada si repetam procesul
            {
                enqueue(Queue,&(*Aux)->Top_left);
                enqueue(Queue,&(*Aux)->Top_right);
                enqueue(Queue,&(*Aux)->Bottom_right);
                enqueue(Queue,&(*Aux)->Bottom_left);
            }
            

        }
    free(Queue);
}

int check(Quadtree **Node, int nr)
{

    if((*Node)->position == nr)
        {
            if((*Node)->Top_left)
                return 1;
            else
                return 0;
        }
    else
        {
            if((*Node)->Top_left)
                {
                    if(check(&(*Node)->Top_left,nr) || check(&(*Node)->Top_right,nr) || check(&(*Node)->Bottom_left,nr) || check(&(*Node)->Bottom_right,nr))
                        return 1;
                }
            else
                return 0;
        }
    return 0;
}

/*Quadtree *search(Quadtree **Node, int nr)
{
    
    if((*Node)->position == nr)
        return (*Node);
    else if((*Node)->Top_left)
        {
            if(search(&(*Node)->Top_left,nr))
                return (*Node)->Top_left;  
            else if(search(&(*Node)->Top_right,nr))
                return (*Node)->Top_right;
            else if(search(&(*Node)->Bottom_right,nr))
                return (*Node)->Bottom_right;
            else if(search(&(*Node)->Bottom_left,nr))
                return (*Node)->Bottom_left;    
        } 
    else return NULL;
    return NULL; 
}*/

void Create_Array(QuadtreeNode *v, Quadtree **Node)//Create the array
{
    queue_t *Queue = initQtypueue();
    enqueue(Queue,Node);
    while(Queue->len > 0)//cat timp avem elemente in coada
        {
            Quadtree **Aux = dequeue(&Queue);
            if((*Aux)->Top_left !=NULL) //verificam daca e nod parinte
                {

                    v[(*Aux)->position].top_left = (*Aux)->Top_left->position;
                    v[(*Aux)->position].top_right = (*Aux)->Top_right->position;
                    v[(*Aux)->position].bottom_right = (*Aux)->Bottom_right->position;
                    v[(*Aux)->position].bottom_left = (*Aux)->Bottom_left->position;
                }
            else 
                {
                    v[(*Aux)->position].top_left = -1; 
                    v[(*Aux)->position].top_right = -1;
                    v[(*Aux)->position].bottom_right = -1;
                    v[(*Aux)->position].bottom_left = -1;
                }
            v[(*Aux)->position].area = (*Aux)->Area;
            v[(*Aux)->position].red = (*Aux)->Pixels.Red;
            v[(*Aux)->position].green = (*Aux)->Pixels.Green;
            v[(*Aux)->position].blue = (*Aux)->Pixels.Blue;
             
            if((*Aux)->Top_right != NULL)
            {
                enqueue(Queue,&(*Aux)->Top_left);
                enqueue(Queue,&(*Aux)->Top_right);
                enqueue(Queue,&(*Aux)->Bottom_right);
                enqueue(Queue,&(*Aux)->Bottom_left);
            }
        }
    free(Queue);
    
}

int count_leaves(QuadtreeNode *v, int nr_elem)//count the number of leaves
{
    int i;
    int nr = 0;
    for(i=0; i<nr_elem; i++)
        if(v[i].top_left == -1)
            nr++;

    return nr;
}

void free_tree(Quadtree **Node)//function to free the quadtree
{
    if((*Node)->Bottom_right != NULL)
        {
            free_tree(&(*Node)->Top_left);
            free_tree(&(*Node)->Top_right);
            free_tree(&(*Node)->Bottom_right);
            free_tree(&(*Node)->Bottom_left);
        }
    free(*Node);
}

void Create_tree(QuadtreeNode *v, Quadtree **Node, int poz)
{
    (*Node)->Area = v[poz].area;
    (*Node)->Pixels.Blue = v[poz].blue;
    (*Node)->Pixels.Green = v[poz].green;
    (*Node)->Pixels.Red = v[poz].red;
    (*Node)->position = poz;
    if(v[poz].top_left != -1) //verificam daca nu e frunza
        {
            //alocam memorie pentru fii si reapelam functia pentru fiecare fiu
            (*Node)->Top_left = malloc(sizeof(Quadtree));
            (*Node)->Top_right = malloc(sizeof(Quadtree));
            (*Node)->Bottom_right = malloc(sizeof(Quadtree));
            (*Node)->Bottom_left = malloc(sizeof(Quadtree));
            Create_tree(v,&(*Node)->Top_left,v[poz].top_left);
            Create_tree(v,&(*Node)->Top_right,v[poz].top_right);
            Create_tree(v,&(*Node)->Bottom_right,v[poz].bottom_right);
            Create_tree(v,&(*Node)->Bottom_left,v[poz].bottom_left);
        }
    else
        {
            (*Node)->Top_right = NULL;
            (*Node)->Top_left = NULL;
            (*Node)->Bottom_left = NULL;
            (*Node)->Bottom_right = NULL;
        }
}

void decompress( Quadtree **Node, Pixel ***Img_Matrix, int x, int y, int latura)
{
    int i,j;

    if((*Node)->Top_left == NULL) //verificam daca e frunza
        {
            //introducem informatiile necesare in matrice
            for(i=y; i<y+latura; i++)
                for(j=x; j<x+latura; j++)
                    {
                        (*Img_Matrix)[i][j].Red = (*Node)->Pixels.Red;
                        (*Img_Matrix)[i][j].Green = (*Node)->Pixels.Green;
                        (*Img_Matrix)[i][j].Blue = (*Node)->Pixels.Blue;
                    }
        }
    else
        {
            //altfel continuam cu fii nodului curent si coordonatele si dimensiunea subimaginii
            decompress(&(*Node)->Top_left,Img_Matrix,x,y,latura/2);
            decompress(&(*Node)->Top_right,Img_Matrix,x+latura/2,y,latura/2);   
            decompress(&(*Node)->Bottom_right,Img_Matrix,x+latura/2,y+latura/2,latura/2); 
            decompress(&(*Node)->Bottom_left,Img_Matrix,x,y+latura/2,latura/2);
        }
}

int main(int argc, char **argv)
{
    if(strcmp(argv[1],"-c") == 0)
        {
            int width = 0;
            int height = 0;
            int max_value;
            char file_type[5];
            int i;

            Pixel **Img_Matrix;
            FILE *f = fopen (argv[3], "rb");

            if(f == NULL)
                printf("Error");

            fgets(file_type,5,f);
            fscanf(f,"%d",&width);
            fscanf(f,"%d\n",&height);
            fscanf(f,"%d",&max_value);

            fseek(f,1,SEEK_CUR); //jump the whitespace
        
            //Allocating memory for the matrix
            Img_Matrix = malloc(height * sizeof(*Img_Matrix));
            for(i = 0; i < height; i++)
                Img_Matrix[i] = malloc(width * sizeof(**Img_Matrix));
        
            for(i=0; i < height; i++)
                fread(Img_Matrix[i],sizeof(Pixel),width,f);//store the pixels in a matrix

            Quadtree *Node = NULL;
            
            int p = 0;
            int factor = atoi(argv[2]);

            queue_t *Q = initQtypueue(); 
            
            Parse_Image(Img_Matrix,&Node,factor, 0, 0, width);//compress the image
            indexing(Q,&Node,&p);//index the tree

            QuadtreeNode *v = malloc(count(&Node)*sizeof(QuadtreeNode));//Malloc the array
            Create_Array(v,&Node);//Create the array

            /*for(i=0; i<count(&Node,0); i++)
                {
                    printf("Node:%d\n",i);
                    printf("%"PRId32"\n",v[i].top_left);
                    printf("%"PRId32"\n",v[i].top_right);
                    printf("%"PRId32"\n",v[i].bottom_right);
                    printf("%"PRId32"\n",v[i].bottom_left);
                }*/
             
            int numar_noduri = count(&Node);
            int numar_culori = count_leaves(v,numar_noduri);

            /*
            printf("colors= %d\n",numar_culori);
            printf("size= %d\n",numar_noduri);
            for(i=0; i<numar_noduri; i++)
                {
                    printf("%d\n",i);
                    printf("%d %d %d\n",v[i].red,v[i].green,v[i].blue);
                    printf("%d\n",v[i].area);
                    printf("%"PRId32"",v[i].top_left);
                    printf("%"PRId32"",v[i].top_right);
                    printf("%"PRId32"",v[i].bottom_right);
                    printf("%"PRId32"\n\n",v[i].bottom_left);
                }
            */

            FILE *out = fopen(argv[4], "wb");
            if(out == NULL)
                printf("Error");
           
            fwrite(&numar_culori,sizeof(int),1,out);
            fwrite(&numar_noduri,sizeof(int),1,out);
            for(i=0; i<numar_noduri; i++)
                {
                    fwrite(&v[i],sizeof(QuadtreeNode),1,out);
                    //printf("%d %d %d %d\n",i,v[i].red,v[i].green,v[i].blue);
                }
                
            //free the tree, matrix and array
            free_tree(&Node);

            for(i=0; i<height; i++)
                free(Img_Matrix[i]);
            free(Img_Matrix);

            free(v);
            //close files
            fclose(f);
            fclose(out);
        }
    else if(strcmp(argv[1],"-d") == 0)
        {
            FILE *f = fopen(argv[2],"rb");
            int nr_frunze;
            int nr_noduri;
            int i,j;
            Pixel **Img_Matrix;
            fread(&nr_frunze,sizeof(int),1,f);
            fread(&nr_noduri,sizeof(int),1,f);

            QuadtreeNode *v = malloc(nr_noduri * sizeof(QuadtreeNode));//malloc the array
            
            fread(v,sizeof(QuadtreeNode),nr_noduri,f);

            Quadtree *Root = NULL;

            Root = malloc(sizeof(Quadtree));

            //printf("%d",v[0].area);
            Create_tree(v,&Root,0);
            
            Img_Matrix = malloc((int)sqrt(v[0].area) * sizeof(*Img_Matrix));//allocating memory for the matrix
            for(i=0; i<(int)sqrt(v[0].area); i++)
                Img_Matrix[i] = malloc((int)sqrt(v[0].area) * sizeof(**Img_Matrix));

            decompress(&Root,&Img_Matrix,0,0,(int)sqrt(v[0].area));   

            //write the result in a new file

            FILE *ppm = fopen(argv[3],"wb");

            fprintf(ppm,"P6\n");
            fprintf(ppm,"%d %d\n",(int)sqrt(v[0].area),(int)sqrt(v[0].area));
            fprintf(ppm,"255\n");

            for(i=0; i<(int)sqrt(v[0].area); i++)
                for(j=0; j<(int)sqrt(v[0].area); j++)
                fwrite(&Img_Matrix[i][j],sizeof(Pixel),1,ppm);
            
            free_tree(&Root);
            //free the tree, matrix and array
            for(i=0; i<(int)sqrt(v[0].area); i++)
                free(Img_Matrix[i]);
            free(Img_Matrix);
            free(v);
            //close files
            fclose(f);
            fclose(ppm);
        }

    return 0;
}