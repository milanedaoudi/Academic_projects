// See LICENSE for license details.

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <memory.h>
#include "fast_math.h"

#include "encoding.h"
#include "mini-printf.h"
#include "diskio.h"
#include "ff.h"
#include "bits.h"
#include "hid.h"
#include "eth.h"
#include "elfriscv.h"
#include "lowrisc_memory_map.h"

// For the CNN application ----
#include "types.h"
#include "top_cnn_mancini.h"
#include "coeffs_cifar.h"
#include "biases_cifar.h"

// Including paramter (sizes, images to read, number of filters ...)
#include "date2020_config.h"


extern unsigned char OVERLAYS_LIST[];
//-----------------------------

#define DEBUG 0
#define DEBUG_PRINTF(...)  \
  do                       \
  {                        \
    if (DEBUG)             \
      printf(__VA_ARGS__); \
  } while (0)

//-----------------------------

FATFS FatFs; // Work area (file system object) for logical drive


// Informations of the read images
#define CONV_READ_WIDTH 640
#define CONV_READ_HEIGHT 480
#define CONV_READ_SIZE_PPM CONV_READ_WIDTH *CONV_READ_HEIGHT * 3
#define CONV_READ_SIZE_PGM CONV_READ_WIDTH *CONV_READ_HEIGHT
#define CONV_READ_INT_FORMAT float


void *memalign(size_t alignment, size_t size)
{
  char *ptr = malloc(size + alignment);
  return (void *)((-alignment) & (size_t)(ptr + alignment));
}


//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  Fonctions Utiles  /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////




/* Les fonctions suivantes ont ete ajoutees dans ce fichier car nous n'avons pas reussi a les inclures depuis les fichiers du RISC-V */
/* Fonction servant à ajouter une chaine de caractere a une autre */
char *My_strcat(char *dest, const char *src)
{
  char *tmp = dest;

  while (*dest)
    dest++;
  while ((*dest++ = *src++) != '\0')
    ;

  return tmp;
}

/* Fonction servant a transformer un caractere en chiffre (dans les cas ou c'est possible) */
int My_atoi(char *chaine)
{
  int res = 0;
  int i;
  for (i = 0; chaine[i] != '\0'; i++)
  {
    res = res * 10 + chaine[i] - '0';
  }
  return res;
}

/* Fonctions utilisees par My_strtok */
char *My_strpbrk(const char *cs, const char *ct)
{
  const char *sc1, *sc2;

  for (sc1 = cs; *sc1 != '\0'; ++sc1)
  {
    for (sc2 = ct; *sc2 != '\0'; ++sc2)
    {
      if (*sc1 == *sc2)
        return (char *)sc1;
    }
  }
  return NULL;
}

size_t My_strspn(const char *s, const char *accept)
{
  const char *p;
  const char *a;
  size_t count = 0;

  for (p = s; *p != '\0'; ++p)
  {
    for (a = accept; *a != '\0'; ++a)
    {
      if (*p == *a)
        break;
    }
    if (*a == '\0')
      return count;
    ++count;
  }

  return count;
}

/* Variable globale utilisee par My_strtok stockant les token suivants */
char *___mystrtok;

/* Fonction permettant de séparer une chaine de caractere en differents token stockes dans __strtok 
   Utilisation : Token = strtok(chaine de caractere, separateur)
                 Token suivant = strtok(NULL, separateur) */
char *My_strtok(char *s, const char *ct)
{

  char *sbegin, *send;

  sbegin = s ? s : ___mystrtok;
  if (!sbegin)
  {
    return NULL;
  }
  sbegin += My_strspn(sbegin, ct);
  if (*sbegin == '\0')
  {
    ___mystrtok = NULL;
    return (NULL);
  }
  send = My_strpbrk(sbegin, ct);
  if (send && *send != '\0')
    *send++ = '\0';
  ___mystrtok = send;
  return (sbegin);
}

// Cifar Related Results  -------------------------------------

unsigned int cifar_class[1];        // Contains the name of the result class
image_type cifar_probabilities[10]; // Contains the probability of each class
const char Cifar10Base[10][11] = {  // Contains the name of each class
    "Airplane",
    "Automobile",
    "Bird",
    "Cat",
    "Deer",
    "Dog",
    "Frog",
    "Horse",
    "Ship",
    "Truck"};

//--------------------------------------------------------------

extern volatile uint64_t *const hid_new_vga_ptr; // = (volatile uint64_t *)(new_vga_base_addr);

uint8_t TAB_GS[NB_IMAGES_TO_BE_READ][DISPLAY_IMAGE_SIZE] = {0};          //Tableau de pixel de toutes les images rangeais les uns apres les autres
uint8_t TAB_GS_FILTERED[NB_IMAGES_TO_BE_READ][DISPLAY_IMAGE_SIZE] = {0}; //Tableau de pixel de toutes les images rangeais les uns apres les autres

// CNN Stuff --------------------------------------------------------------------
// Tableau de pixel de toutes les images rangés les uns apres les autres
uint8_t global_tab[NB_IMAGES_TO_BE_READ * DISPLAY_IMAGE_SIZE * 3] = {0};

uint8_t resized_img[NN_IN_SIZE * 3] = {0};
float resized_tensor[NN_IN_SIZE * 3] = {0};
float normalized_tensor[NN_IN_SIZE*3] = {0};
float alized_tensor[NN_IN_SIZE * 3] = {0};
// -------------------------------------------------------------------------------------



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  Partie Lecture des images  //////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// -------------MARTIN-------------------
void read_pic(int n_image,
              int *tab_size,
              int *tab_width,
              int *tab_length,
              uint8_t *global_tab)
{
    FIL fil;
    FRESULT fr;
    uint32_t br;


    char file_name[32];
    char *line = NULL;
    uint8_t *pixels = NULL;

    int c1 = 0, c2 = 0;
    int width = 0, height = 0;
    int size = 0;

    int idx = n_image - 1;

    /* Build filename safely */
    snprintf(file_name, sizeof(file_name), "%d.ppm", n_image);
    printf("[read_pic] Loading %s\n", file_name);

    line = calloc(10000, 1);
    if (!line)
    {
        printf("ERROR: line buffer allocation failed\n");
        goto cleanup;
    }

    /* Open file */
    fr = f_open(&fil, file_name, FA_READ);
    if (fr)
    {
        printf("ERROR: cannot open %s\n", file_name);
        goto cleanup;
    }

    /* Read magic number */
    f_read(&fil, &c1, 1, &br);
    f_read(&fil, &c2, 1, &br);

    if (c1 != 'P' || c2 != '3')
    {
        printf("ERROR: %s is not a PPM P3 file\n", file_name);
        goto cleanup_file;
    }

    /* Skip end of line */
    if (!f_gets(line, 10000, &fil))
        goto cleanup_file;

    /* Skip comments */
    do
    {
        if (!f_gets(line, 10000, &fil))
            goto cleanup_file;
    } while (line[0] == '#');

    /* Read width & height */
    char *tok = My_strtok(line, " ");
    if (!tok) goto cleanup_file;
    width = My_atoi(tok);

    tok = My_strtok(NULL, "\n");
    if (!tok) goto cleanup_file;
    height = My_atoi(tok);

    size = width * height;

    printf("[read_pic] Image %d : %dx%d (%d pixels)\n",
           n_image, width, height, size);

    tab_width[idx]  = width;
    tab_length[idx] = height;
    tab_size[idx]   = size;

    /* Validate resolution */
    if (size != DISPLAY_IMAGE_SIZE)
    {
        printf("ERROR: image size mismatch! got=%d expected=%d (must be 640x480)\n",
               size, DISPLAY_IMAGE_SIZE);
        goto cleanup_file;
    }

    /* Read max color value */
    if (!f_gets(line, 10000, &fil))
        goto cleanup_file;

    /* Allocate pixel buffer (aligned) */
    pixels = memalign(4, DISPLAY_IMAGE_SIZE * 3);
    if (!pixels)
    {
        printf("ERROR: pixel buffer allocation failed\n");
        goto cleanup_file;
    }

    /* Read pixel data */
    int i = 0;
    while (i < DISPLAY_IMAGE_SIZE * 3 && f_gets(line, 10000, &fil))
    {
        tok = My_strtok(line, " ");
        while (tok && i < DISPLAY_IMAGE_SIZE * 3)
        {
            pixels[i++] = (uint8_t)My_atoi(tok);
            tok = My_strtok(NULL, " ");
        }
    }

    if (i != DISPLAY_IMAGE_SIZE * 3)
    {
        printf("ERROR: incomplete pixel data read (%d bytes)\n", i);
        goto cleanup_file;
    }

    /* Copy into global_tab safely */
    int base = idx * DISPLAY_IMAGE_SIZE * 3;

    for (i = 0; i < DISPLAY_IMAGE_SIZE * 3; ++i)
    {
        global_tab[base + i] = pixels[i];
    }

    printf("[read_pic] Image %d loaded successfully\n", n_image);

cleanup_file:
    f_close(&fil);

cleanup:
    if (pixels) free(pixels);
    if (line) free(line);
}




//---------------Jerem-----------------------
// ------------ GREYSCALE--------------------
//-------------------------------------------
void convert_to_greyscale(
	int n_image, 
	int *tab_size, 
	int *tab_width, 
	int *tab_length, 
	uint8_t *global_tab, 
	uint8_t image[CONV_READ_SIZE_PGM])
{
  printf("Affichage image numero : %d   %d*%d=%d\n", n_image, tab_width[n_image - 1], tab_length[n_image - 1], tab_size[n_image - 1]);
  
  // I / On calcule l'offset pour la conversion de RGB à Greyscale	 
  int offset = 0; 
  for (int i = 0; i < n_image - 1; i++) {
        offset += tab_size[i] * 3;
  }
  // II/ On transforme l'image RGB en greyscale

  for (int i = 0; i < tab_size[n_image - 1] * 3; i += 3)
  { 	
	// III/ On crée les variables qui vont stocker R,G,B.
	uint8_t R = global_tab[offset + i];
        uint8_t G = global_tab[offset + i + 1];
        uint8_t B = global_tab[offset + i + 2];
	
        // IV / On converti avec les coefficients proposés.
        uint8_t grey = (uint8_t)(
            0.3f  * R + 0.57f * G + 0.11f * B 
        );

        // V / On attribue le gris à l'image correspondante.
        image[i / 3] = grey;
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////  Partie Interruptions  ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// FOR INTERRUTPS  ------------------------------------------

void init_csrs()
{
 void init_csrs()
{
  //Désactivation de toutes les int
  clear_csr(mstatus, MSTATUS_MIE);

  // Activation MIE
  set_csr(mie, MIP_MEIP);

  // Pas d'appel du superviseur
  write_csr(mideleg, 0);
  write_csr(medeleg, 0);

  // Clear les int en attente
  write_csr(mip, 0);
  write_csr(sip, 0);
}

}
/*
#define PLIC_BASE_ADDRESS 0x0C000000
#define PLIC_MAX_PRIORITY 7

#define ID_BTNW 1
#define ID_BTNE 2
#define ID_BTNS 3
#define ID_BTNN 4

#define PLIC_PRIORITY_BTNW (PLIC_BASE_ADDRESS + 4 * ID_BTNW)
#define PLIC_PRIORITY_BTNE (PLIC_BASE_ADDRESS + 4 * ID_BTNE)
#define PLIC_PRIORITY_BTNS (PLIC_BASE_ADDRESS + 4 * ID_BTNS)
#define PLIC_PRIORITY_BTNN (PLIC_BASE_ADDRESS + 4 * ID_BTNN)

#define PLIC_INT_PENDING_BASEADDR 0x0C001000
#define PLIC_INT_ENABLE_BASEADDR 0x0C002000

#define PLIC_HART0_PRIO_THRESH_ADDR 0x0C200000
#define PLIC_HART0_CLAIM_COMPLETE_ADDR 0x0C200004

// Masks definition
// Refers to chip_top.sv to know the connections of the buttons
#define PLIC_PENDING_BTNW (1 << 1)
#define PLIC_ENABLE_BTNW (1 << 1)

#define PLIC_PENDING_BTNE (1 << 2)
#define PLIC_ENABLE_BTNE (1 << 2)

#define PLIC_PENDING_BTNS (1 << 3)
#define PLIC_ENABLE_BTNS (1 << 3)

#define PLIC_PENDING_BTNN (1 << 4)
#define PLIC_ENABLE_BTNN (1 << 4)

*/



void enable_plic_interrupts(void)
{
    // Set priority of each button
    *(volatile uint32_t *)PLIC_PRIORITY_BTNW = 1;
    *(volatile uint32_t *)PLIC_PRIORITY_BTNE = 1;
    *(volatile uint32_t *)PLIC_PRIORITY_BTNS = 1;
    *(volatile uint32_t *)PLIC_PRIORITY_BTNN = 1;

    // Set threshold to 0
    *(volatile uint32_t *)PLIC_HART0_PRIO_THRESH_ADDR = 0;

    // Clear pending interrupts
    *(volatile uint32_t *)PLIC_INT_PENDING_BASEADDR = 0;

    // Enable button interrupts
    *(volatile uint32_t *)PLIC_INT_ENABLE_BASEADDR =
        PLIC_ENABLE_BTNW | PLIC_ENABLE_BTNE | PLIC_ENABLE_BTNS | PLIC_ENABLE_BTNN;

    // Enable external interrupt in machine
    set_csr(mie, MIP_MEIP);
    set_csr(mstatus, MSTATUS_MIE);
}





//volatile int imageSel;
/*
volatile int filterSel; 
volatile int isBouncing;
extern uint32_t imageSel;


void bouton(void)
{
    // Read the claim register (highest priority pending interrupt)
    int claim = plic[PLIC_HART0_CLAIM_COMPLETE_ADDR / 4];
    clear_csr(mie, MIP_MEIP); // Disable external interrupts during handler

    printf("[DEBUG] Interrupt claim: %d\n", claim);

    if (isBouncing == 0)
    {
        switch (claim)
        {
            case ID_BTNW:
                imageSel--;
                if (imageSel < 0) imageSel = NB_IMAGES_TO_BE_READ - 1;
                printf("[DEBUG] BTN W pressed -> imageSel=%d\n", imageSel);
                break;

            case ID_BTNE:
                imageSel++;
                if (imageSel >= NB_IMAGES_TO_BE_READ) imageSel = 0;
                printf("[DEBUG] BTN E pressed -> imageSel=%d\n", imageSel);
                break;

            case ID_BTNS:
                filterSel--;
                if (filterSel < 0) filterSel = CNN_CLASSIFIER;
                printf("[DEBUG] BTN S pressed -> filterSel=%d\n", filterSel);
                break;

            case ID_BTNN:
                filterSel++;
                if (filterSel > CNN_CLASSIFIER) filterSel = BYPASS;
                printf("[DEBUG] BTN N pressed -> filterSel=%d\n", filterSel);
                break;

            default:
                printf("[DEBUG] Unknown interrupt ID: %d\n", claim);
                break;
        }

        isBouncing = 1;
    }
    else
    {
        printf("[DEBUG] Button bounce ignored\n");
    }

    // Complete the interrupt (tell PLIC we handled it)
    plic[PLIC_HART0_CLAIM_COMPLETE_ADDR / 4] = claim;

    // Re-enable external interrupts
    set_csr(mie, MIP_MEIP);
}

*/






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





// MACROS
#define indexCalculationCONV(i, j, c, size_i, size_j, size_c) (i + j * size_i + c * size_i * size_j)




// Filter type enum


/* CONVOLUTION */
#define CONV_CONV_SIZE_0 640
#define CONV_CONV_SIZE_1 480
#define CONV_CONV_SIZE_2 1
#define CONV_CONV_TOTAL_SIZE CONV_CONV_SIZE_0 *CONV_CONV_SIZE_1
#define CONV_CONV_FIXED_FORMAT float
#define CONV_CONV_NORMALIZE 15

/* CONV KERNEL(S) */
#define KERNEL1_CONV_SIZE_L 1
#define KERNEL1_CONV_SIZE_M 3
#define KERNEL1_CONV_SIZE_N 3

#define KERNEL_CONV_FIXED_FORMAT float

#define EDGE_DETECTOR_NORMALIZE (float)0.00194
#define EDGE_DETECTOR_NORMALIZE2 (float)0.0623
#define EDGE_DETECTOR_THRESHOLD 15

/* CONV BIAISES */
#define BIAISES_CONV_FIXED_FORMAT float







///////////////////////////////////////////////////////////////////////////////////
///////////////////// CONVOLUTION SIMPLE FIN //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static KERNEL_CONV_FIXED_FORMAT kernel[] = {-0.125, -0.125, -0.125,
                                            -0.125, 1, -0.125,
                                            -0.125, -0.125, -0.125};

static BIAISES_CONV_FIXED_FORMAT biaises[] = {0};

// filter_nb = soit 0 soit 1
void convolution_filter(uint8_t image[CONV_READ_SIZE_PGM], KERNEL_CONV_FIXED_FORMAT kernel[3 * 3 * 1], BIAISES_CONV_FIXED_FORMAT biaises[1], uint8_t output[CONV_CONV_TOTAL_SIZE])
{
  for (int j = 0; j < CONV_CONV_SIZE_1; j++)
  {
    for (int i = 0; i < CONV_CONV_SIZE_0; i++)
    {
      for (int c = 0; c < CONV_CONV_SIZE_2; c++)
      {
        CONV_CONV_FIXED_FORMAT sum = 0;
        for (int l = 0; l < KERNEL1_CONV_SIZE_L; l++)
        {
          for (int m = 0; m < KERNEL1_CONV_SIZE_M; m++)
          {
          bn:
            for (int n = 0; n < KERNEL1_CONV_SIZE_N; n++)
            {
              if (((j + n) > (CONV_CONV_SIZE_1 - 1)) && ((i + m) < (CONV_CONV_SIZE_0 - 1)))
              {
                sum = sum + 0;
              }
              else if (((i + m) > (CONV_CONV_SIZE_0 - 1)) && ((j + n) < (CONV_CONV_SIZE_1 - 1)))
              {
                sum = sum + 0;
              }
              else if (((i + m) > (CONV_CONV_SIZE_0 - 1)) && ((j + n) > (CONV_CONV_SIZE_1 - 1)))
              {
                sum = sum + 0;
              }
              else if (((i + m) < (CONV_CONV_SIZE_0)) && ((j + n) < (CONV_CONV_SIZE_1)))
              {
                sum = sum + image[indexCalculationCONV((i + m), (j + n), l, (CONV_CONV_SIZE_0), (CONV_CONV_SIZE_1), (CONV_CONV_SIZE_2))] * kernel[m + n * KERNEL1_CONV_SIZE_M + l * KERNEL1_CONV_SIZE_M * KERNEL1_CONV_SIZE_N + c * KERNEL1_CONV_SIZE_M * KERNEL1_CONV_SIZE_N * KERNEL1_CONV_SIZE_L];
              }
            }
          }
        }

        CONV_CONV_FIXED_FORMAT tmp = sum * CONV_CONV_NORMALIZE;

        if (tmp < 0)
        {
          tmp = tmp * (CONV_CONV_FIXED_FORMAT)(-1);
        }

        if (sum < EDGE_DETECTOR_THRESHOLD)
        {
          output[indexCalculationCONV(i, j, c, (CONV_CONV_SIZE_0), (CONV_CONV_SIZE_1), (CONV_CONV_SIZE_2))] = 0;
        }
        else if (sum > 255 || tmp > 255)
        {
          output[indexCalculationCONV(i, j, c, (CONV_CONV_SIZE_0), (CONV_CONV_SIZE_1), (CONV_CONV_SIZE_2))] = 255;
        }
        else
        {
          output[indexCalculationCONV(i, j, c, (CONV_CONV_SIZE_0), (CONV_CONV_SIZE_1), (CONV_CONV_SIZE_2))] = (uint8_t)(tmp + biaises[0]);
        }
      }
    }
  }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Partie CNN  ///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//square root function
/*
float __ieee754_sqrtf(float x)
{
  asm("... %0, %1"
      : "=f"(x)
      : "f"(x));
  return x;
}

double __ieee754_sqrt(double x)
{
  asm("... %0, %1"
      : "=f"(x)
      : "f"(x));
  return x;
}
*/

// This use the AREA based resizing method, just like the one used in OpenCV

//-----------------JEREM---------------------
// ------------ MY_RESIZING--------------------
//-------------------------------------------

// This use the AREA based resizing method, just like the one used in OpenCV
void my_resizing(uint8_t *target_img, uint8_t *source_img, 
                 int source_size, int source_sizeX, int source_sizeY,
                 int target_size, int target_sizeX, int target_sizeY)
{
    // I/ On calcule les échelles de redimensionnement. 
    double x_ratio = source_sizeX / target_sizeX;
    double y_ratio = source_sizeY / target_sizeY;

    double temp = 0.0;  // Somme pondérée des pixels.
    double w = 0.0;     // Somme des poids (aire totale de l'image)

    // II / On parcourt chaque pixel de l'image target
    for (int ty = 0; ty < target_sizeY; ty++) {
        for (int tx = 0; tx < target_sizeX; tx++) {

            // III/ Déterminer la zone correspondante dans l'image source
            double x_start = tx * x_ratio;       // On commence à gauche
            double x_end   = (tx + 1) * x_ratio; // Côté droit de la zone
            double y_start = ty * y_ratio;       // Côté haut de la zone
            double y_end   = (ty + 1) * y_ratio; // Côté bas de la zone

            // IV/ On détermine quels pixels source sont concernés par le resizing
            int x0 = floorf(x_start); // premier pixel source inclus -> On prend la valeur basse de x_start
            int x1 = ceilf(x_end);   // dernier pixel source inclus -> On prend la valeur haute
            int y0 = floorf(y_start);
            int y1 = ceilf(y_end);

            temp = 0.0; // Réinitialiser la somme pour ce pixel cible
            w = 0.0;    // Réinitialiser le poids total

            // V/ On parcourt des pixels source dans la zone
            for (int sy = y0; sy < y1; sy++) {
                for (int sx = x0; sx < x1; sx++) {

                    // VI/ On calcule le "poids" du pixel source
                    // -> C'est la surface de recouvrement entre pixel source et zone cible

		    // -> fmin donne le bord droit de la zone commune et fmax le bord gauche, si on soustrait ça donne la largeur, même raisonnement pour Y
                    double x_overlap = fminf(x_end, sx + 1.0) - fmaxf(x_start, sx); 
                    double y_overlap = fminf(y_end, sy + 1.0) - fmaxf(y_start, sy);
                    double weight = x_overlap * y_overlap;

                    // VII/  On ajoute la contribution du pixel source
                    temp += source_img[sy * source_sizeX + sx] * weight;
                    w += weight;
                }
            }

            // VIII/ On calcule la valeur moyenne pondérée
            target_img[ty * target_sizeX + tx] = temp / w + 0.5;
            // +0.5 pour arrondir correctement au plus proche entier
        }
    }
}



//
// Normalizing the image 24x24 to be feed to the CNN
// ---------------ORDAN--------------
//
float *normalizing(float *target_img, float *source_img, int size) // height * width * 3
{
   for (int i = 0; i < size; ++i){
    target_img[i] = source_img[i] / 255.0f;
  }
  return target_img;
}



// ------------------JEREM-----------------
// Normalizing the image 24x24 to be feed to the CNN
//
	
float *normalizing_tensor(float *target_tensor, float *source_tensor, int size)
{
    int num_pixels = size * size * 3; // Nombre d'élément dans le tenseur, * 3 pour les channels RGB

    for (int i = 0; i < num_pixels; i++) {
        target_tensor[i] = source_tensor[i] / 255.0f; // Normaliser de O à 1 
        // target_tensor[i] = (source_tensor[i] / 127.5f) - 1.0f; Normaliser de -1 à 1 
    }

    return target_tensor;
}


//-------------------------------------------
// ------------ IMG_TO_TENSOR--------------------
//-------------------------------------------
/*
  Converting an RGB image to a tensor, 
    i.e. R0R1R2......G0G1G2........B0B1B2......
*/
void img_to_tensor(float *target_tensor, uint8_t *source_img, int source_size, int source_sizeX, int source_sizeY)
{
    // I/ on calcule le nombre de pixel total
    int num_pixels = source_sizeX * source_sizeY; // -> Nombre de pixel sur la source

    for (int y = 0; y < source_sizeY; y++) {
        for (int x = 0; x < source_sizeX; x++) {
            int img_idx = (y * source_sizeX + x) * 3; // -> On récupère l'index du RGB 
            int idx = y * source_sizeX + x;           // -> Position du pixel dans le channel

            // II/ On sépare les canaux RGB dans une mémoire contigue.
            target_tensor[idx]                  = source_img[img_idx];     // R
            target_tensor[idx + num_pixels]     = source_img[img_idx + 1]; // G
            target_tensor[idx + 2 * num_pixels] = source_img[img_idx + 2]; // B
        }
    }
}


//Cette fonction a été retirée de votre template, mais vous pouvez vous en inspirer pour écrire la votre
/*extern void top_cnn_mancini(coef_type tab_coeffs[NB_COEFFS], coef_type tab_biais[NB_BIAIS], led_type cifar_class[1], image_type image_in[CONV_SIZE_1 * CONV_SIZE_1 * 3], image_type cifar_probabilities[NCAN_OUT_5]);*/  
//------------------ORDAN +JEREM-START--------------------
int perform_cnn(int img_in_number)
{
  // Source image (RGB 640×480) depuis global_tab
  uint8_t *source_img =
    &global_tab[(img_in_number - 1)
                * DISPLAY_IMAGE_SIZE * 3];

  // Resize: 640×480 → 24×24 RGB
  DEBUG_PRINTF("Starting resizing\n");
  my_resizing(resized_img,
              source_img,
              DISPLAY_IMAGE_SIZE * 3,
              DISPLAY_IMAGE_WIDTH,
              DISPLAY_IMAGE_HEIGHT,
              NN_IN_SIZE * 3,
              NN_IN_WIDTH,
              NN_IN_HEIGHT);

  // RGB interleaved → planar tensor
  DEBUG_PRINTF("Starting img_to_tensor\n");
  img_to_tensor(resized_tensor,
                resized_img,
                NN_IN_SIZE * 3,
                NN_IN_WIDTH,
                NN_IN_HEIGHT);

  // Normalisation
  DEBUG_PRINTF("Starting normalization\n");
  normalizing_tensor(normalized_tensor,
                     resized_tensor,
                     NN_IN_SIZE * 3);

  // CNN inference
  top_cnn_mancini(tab_coeffs,
                  tab_biais,
                  cifar_class,
                  normalized_tensor,
                  cifar_probabilities);

  // Debug prints
  printf("\nairplane :    %d \n", (int)cifar_probabilities[0]);
  printf("automobile :  %d \n", (int)cifar_probabilities[1]);
  printf("bird :        %d \n", (int)cifar_probabilities[2]);
  printf("cat :         %d \n", (int)cifar_probabilities[3]);
  printf("deer :        %d \n", (int)cifar_probabilities[4]);
  printf("dog :         %d \n", (int)cifar_probabilities[5]);
  printf("frog :        %d \n", (int)cifar_probabilities[6]);
  printf("horse :       %d \n", (int)cifar_probabilities[7]);
  printf("ship :        %d \n", (int)cifar_probabilities[8]);
  printf("truck :       %d \n", (int)cifar_probabilities[9]);

  printf("--> The image type is %s with a probability of : %d \n\n",
         Cifar10Base[cifar_class[0]],
         (int)cifar_probabilities[cifar_class[0]]);

  return cifar_class[0];
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  Partie Display  /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



//------------------ORDAN-START--------------------
void display(int img_in_number, filter_type filter_nb, uint8_t previous_imageSel, uint8_t previous_filterSel) //, uint8_t *edgeDetectorDone, uint8_t *CNNDone)
{
  volatile uint64_t *display_ptr;
  volatile uint64_t *diplay_ptr_filtered;
  volatile uint8_t *ptr_selected_img;
  volatile uint8_t *ptr_selected_img_filtered;

  // Get the image to print
  display_ptr = (uint64_t *)(TAB_GS[img_in_number - 1]);
  ptr_selected_img = (uint8_t *)(TAB_GS[img_in_number - 1]);
  diplay_ptr_filtered = (uint64_t *)(TAB_GS_FILTERED[img_in_number - 1]);
  ptr_selected_img_filtered = (uint8_t *)(TAB_GS[img_in_number - 1]);

  int x, y;

  switch (filter_nb)				//Disjonction de cas en fonction du filtre sélectionné
  {

  case BYPASS:
    on_screen(BYPASS, 0, TAB_GS[img_in_number - 1]);     //image greyscale brute
    break;

  case EDGE_DETECTOR:
    on_screen( EDGE_DETECTOR, 0, TAB_GS_FILTERED[img_in_number - 1] );    //Image déjà filtrée
    break;

  case CNN_CLASSIFIER:
    // In this case we visualize the image, while computing ...
    on_screen(BYPASS, 0, TAB_GS[img_in_number - 1]);   //Afficher l'image brute pendant le calcul
    /*for (y = 0; y < 480; ++y)
    {
      for (x = 0; x < 640 / 8; ++x)
      {
        ...  = ... ;
	... ;
      }
    }*/
    // Launch the CNN
    int result = perform_cnn(img_in_number);
    // When finished, show the LABEL as an overlay.
    on_screen(CNN_CLASSIFIER, result, TAB_GS[img_in_number - 1]);
    break;
  }
}




//-------------ORDAN--------------

void on_screen(int mode, int class, uint8_t *img)
{ //, uint8_t* proc_img){
  printf("Welcome to on_screen\n");

  int y, x;
  int y_offset, x_offset;
  volatile uint64_t *ptr_image = (uint64_t *)(img);
  volatile uint64_t *ptr_labels_overlay = (uint64_t *)(OVERLAYS_LIST);

  if (mode == BYPASS)										//Sélection de l'étiquette en fonction du filtre choisi
  {
    printf("\nPainting BYPASS overlay.\n");
    //L'image à l'indice 10 correspond à l'overlay du bypass
    ptr_labels_overlay = (uint64_t *)(OVERLAYS_LIST + 10 * OVERLAY_SIZE); // on decale pour sauter les etiquettes des classes du CNN
    y_offset = 0;
    x_offset = 0;
  }
  else if (mode == CNN_CLASSIFIER)
  {
    printf("\nPainting CNN CLASS overlay\n");
    //L'image aux indices 0 à 9 correspondent aux overlays des différentes classes du CNN
    ptr_labels_overlay = (uint64_t *)(OVERLAYS_LIST + class * OVERLAY_SIZE);
    y_offset = 0;
    x_offset = 0;
  }
  else if (mode == EDGE_DETECTOR)
  {
    printf("\nPainting the FILTER overlay\n");
    //L'image à l'indice 11 correspond à l'overlay du edge detector
    ptr_labels_overlay = (uint64_t *)(OVERLAYS_LIST + 11 * OVERLAY_SIZE); //apres les etiquettes des classes
    y_offset = 0;
    x_offset = 0;
  }

  for (y = 0; y < 480; ++y)						//Affichage de l'image
  {
    for (x = 0; x < 640 / 8; ++x)
    {
      if (x < OVERLAY_WIDTH/8 && y < OVERLAY_HEIGHT )
      { //on verifie si on est dans la zone de l'etiquette
        hid_new_vga_ptr[x + y * 640 / 8] = (*ptr_labels_overlay);      //un mot (1 octet) de l’overlay
        ptr_labels_overlay++;
      }
      else
      {
        hid_new_vga_ptr[x + y * 640 / 8] = (*ptr_image);
      }
      ptr_image++;
    }
  }
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////  Fonction Main //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





int main(void)
{
    init_csrs();
    printf("========== INIT BUTTONS & INTERRUPTS ======\n");

    enable_plic_interrupts();

    printf("Vous pouvez tester les boutons maintenant\n");

    while (1)
    {
        // Display current selection
        printf("imageSel=%d filterSel=%d\n", imageSel, filterSel);

        // Simple delay
        for (volatile int i = 0; i < 1000000; i++);
    }
}











///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void about()
{
  printf("----- DEMO DATE 2020 - University Booth --------\n");
  printf(" by Noureddine Ait Said, and the PHELMA students\n");
  printf("          Supervised by Mounir Benabdenbi       \n");
  printf("            AMfoRS Team, TIMA Laboratory        \n");
  printf("------------------------------------------------\n");
  printf("      Version 1.0  Built @" __TIMESTAMP__ "     \n");
  printf("------------------------------------------------\n");
  printf("\n");
  printf("(#################################################\n");
  printf("(#################################################\n");
  printf("(###########,,/((//*,*/.(###((######(#############\n");
  printf("(##########*/((((((((((((((((((*,*/((((,(#########\n");
  printf("(##########,((((((((((((((((((((((((((((.#########\n");
  printf("(#########,/((((((((((((((((((((((((((((*.,,,,,*##\n");
  printf("(#####(**,/(((((((((((((((((((((((((((((((((((((/,\n");
  printf("(###(./(((((((((((((((((((((((((((((((((((((((((((\n");
  printf("(,/((((((((((((((((((((((((((((((((((((((/,       \n");
  printf("/*((((((((((/////((((((((((((((((///(((.  ./(,    \n");
  printf(" /(((((((////////((((((((((((/.      ////,   ,    \n");
  printf("/(/*,     ,//////(((///.      .*,    *.   *(*     \n");
  printf("          ,/(//*/*     .,    ,((*    ,     ./((((/\n");
  printf("   .*((/*./*    /*    /((.   ,((*    /(((((((((((/\n");
  printf("/(((/     /*    /*    /((.   ,(((((((((((((((((((/\n");
  printf("/(((/     /*    /*    /((((((((((((((((((((((((((/\n");
  printf("/(((/     /*    ///((((((((((((((((((((((((((((((/\n");
  printf("/(((/     /((((((((((((((((((((((((((((((((((((((/\n");
  printf("/((((/(((((((((((((((((((((((((((((((((((((((((((/\n\n\n");
}





int lowrisc_init(unsigned long addr, int ch, unsigned long quirks);
void tohost_exit(long code);

unsigned long get_tbclk(void)
{
  unsigned long long tmp = 1000000;
  return tmp;
}

char *env_get(const char *name)
{
  return (char *)0;
}

int init_mmc_standalone(int sd_base_addr);

DSTATUS disk_initialize(uint8_t pdrv)
{
  printf("\nu-boot based first stage boot loader\n");
  init_mmc_standalone(sd_base_addr);
  return 0;
}

int ctrlc(void)
{
  return 0;
}

void *find_cmd_tbl(const char *cmd, void *table, int table_len)
{
  return (void *)0;
}

unsigned long timer_read_counter(void)
{
  return read_csr(0xb00) / 10;
}

void __assert_fail(const char *__assertion, const char *__file,
                   unsigned int __line, const char *__function)
{
  printf("assertion %s failed, file %s, line %d, function %s\n", __assertion, __file, __line, __function);
  tohost_exit(1);
}

/*
void *memalign(size_t alignment, size_t size)
{
  char *ptr = malloc(size + alignment);
  return (void *)((-alignment) & (size_t)(ptr + alignment));
}
*/

int do_load(void *cmdtp, int flag, int argc, char *const argv[], int fstype)
{
  return 1;
}

int do_ls(void *cmdtp, int flag, int argc, char *const argv[], int fstype)
{
  return 1;
}

int do_size(void *cmdtp, int flag, int argc, char *const argv[], int fstype)
{
  return 1;
}

DRESULT disk_read(uint8_t pdrv, uint8_t *buff, uint32_t sector, uint32_t count)
{
  while (count--)
  {
    read_block(buff, sector++);
    buff += 512;
  }
  return FR_OK;
}

DRESULT disk_write(uint8_t pdrv, const uint8_t *buff, uint32_t sector, uint32_t count)
{
  return FR_INT_ERR;
}

DRESULT disk_ioctl(uint8_t pdrv, uint8_t cmd, void *buff)
{
  return FR_INT_ERR;
}

DSTATUS disk_status(uint8_t pdrv)
{
  return FR_INT_ERR;
}

void part_init(void *bdesc)
{
}

void part_print(void *desc)
{
}

void dev_print(void *bdesc)
{
}

unsigned long mmc_berase(void *dev, int start, int blkcnt)
{
  return 0;
}

unsigned long mmc_bwrite(void *dev, int start, int blkcnt, const void *src)
{
  return 0;
}

const char version_string[] = "LowRISC minimised u-boot for SD-Card";
