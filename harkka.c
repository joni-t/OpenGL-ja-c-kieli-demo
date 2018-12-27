#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef CALLBACK
#define CALLBACK
#endif

#define PIKSELEITA 200	// 2D-rasteritoteutus
#define PI 3.14159265358979323846
#define VALOJA 3

typedef struct{	//tietotyyppi BMPLoader:lle
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
} Image;
#include "BMPLoader.c"
#define AITA_TEX "aita.bmp"

//modet
static int mode = 1;
static int KaksiD_1 = 1;
static int KaksiD_2 = 2;
static int kaivuri = 3;

static int width, height;

//***rasteritoteutus***//
static int pikseleita = PIKSELEITA;	//pikseleita vaaka- ja pystysuunnassa
static int raja_leveys = 1;
static int rata_leveys = 5;
static int ratoja = 8;
//nurmikentta
static int kentan_kp_x = PIKSELEITA / 2;
static int kentan_kp_y = PIKSELEITA / 2;
static int kentan_pituus = 80;
static int kentan_leveys = 40;
//varit
static int tausta = 0;
static int viherio = 1;
static int juoksurata = 2;
static int rataviivat = 3;
static GLfloat varit[4][3] = {{ 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0 },
	{ 1.0, 0.0, 0.0 },
	{ 1.0, 1.0, 1.0 }};

static int kuva[PIKSELEITA][PIKSELEITA];
static float poly_jpk[][2] = {	//jalkapallokentta
    { 0.0, 0.0 },
    { 0.0, 40.0 },
    { 80.0, 40.0 },
    { 80.0, 0.0 }
};
static float poly_jpk_rajat[][2] = {	//jalkapallokentta
    { 5.0, 5.0 },
    { 75.0, 5.0 },
    { 75.0, 35.0 },
    { 5.0, 35.0 },
    { 5.0, 5.0 },
    { 40.0, 5.0 },
    { 40.0, 35.0 },
    { 40.0, 5.0 }
};

//***kaivurin maaritykset***//
static float tela_sx = 1.2, tela_sy = 0.27, tela_sz = 0.24;
static float runko1_sx = 1.2, runko1_sy = 1.2, runko1_sz = 0.1;
static float runko2_sx = 0.36, runko2_sy = 0.36, runko2_sz = 0.12;
static float runko3_sx = 1.50, runko3_sy = 1.20, runko3_sz = 0.27;
static float koppi_sx = 0.75, koppi_sy = 0.60, koppi_sz = 0.48;
static float pakoputki_sx = 0.12, pakoputki_sy = 0.12, pakoputki_sz = 0.60;
static float varsi_y_sx = 1.14, varsi_y_sy = 0.15, varsi_y_sz = 0.15;
static float varsi_a_sx = 1.00, varsi_a_sy = 0.12, varsi_a_sz = 0.12;
static float kauha_sx = 0.40, kauha_sy = 0.12, kauha_sz = 0.21;
static float kulma_varsi_y = -45.0, kulma_varsi_a = 150, kulma_kauha = 120;
static float sijainti_x = 0.25, sijainti_y = 1.0;
static float suuntakulma = 0.00, kiertyma = 0.00;
static int tyonopeus = 5;
//***halli, kaivuri ja aita***
//hallin katto ja sivuseinat
GLfloat halli_cps1[6][2][3] = {	//[pistejonojen lkm][pisteiden lkm pistejonossa][pisteen koordinaattien lkm]
        {{2.00, 0.00, 0.00},	// vasen reuna [0][j][k]
	 {0.00, 0.00, 0.00}},

	 {{2.00, 0.2, 1.4},
	 {0.00, 0.2, 1.4}},

	 {{2.00, 0.52, 1.5},
	 {0.00, 0.52, 1.5}},

	{{2.00, 1.48, 1.50},
	 {0.00, 1.48, 1.50}},

	 {{2.00, 1.8, 1.4},
	 {0.00, 1.8, 1.4}},

	 {{2.00, 2.00, 0.0},	// oikea reuna [5][j][k]
	 {0.00, 2.00, 0.0}}	
};
//takaseina
GLfloat halli_cps2[3][4][3] = {	//[pistejonojen lkm][pisteiden lkm pistejonossa][pisteen koordinaattien lkm]
        {{0.00, 0.00, 0.00},	// alareuna [0][j][k]
	 {-0.30, 0.67, 0.0},
	 {-0.30, 1.34, 0.00},
	 {0.00, 2.0, 0.00}},

	 {{0.00, 0.20, 1.20},
	 {-0.25, 0.8, 1.20},
	 {-0.25, 1.6, 1.20},
	 {0.00, 1.8, 1.20}},

	 {{0.00, 0.52, 1.30},	// ylareuna [2][j][k]
	 {0.0, 0.32, 1.30},
	 {0.0, 0.64, 1.30},
	 {0.0, 0.96, 1.30}}	
};
GLUnurbsObj *Nurb_halli;
//aita
static float aita_coords[4][3] = {
    { 2.0, 2.0,  0.0 }, /* 0 */
    { 2.0, 2.0,  1.0 }, /* 1 */
    { 2.0, 4.0,  1.0 }, /* 2 */
    { 2.0, 4.0,  0.0 } /* 3 */
};
static float aita_normaali[1][3] = {
    { 1.0, 0.0,  0.0 }
};
static float aita_t[4][2] = {	//texturekoordinaatit
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 }
};
//kaivurin peruspalikka
static float box_coords[][3] =
{
    {-1.0, -1.0,  1.0 }, /* 0 */
    { 1.0, -1.0,  1.0 }, /* 1 */
    { 1.0, -1.0, -1.0 }, /* 2 */
    {-1.0, -1.0, -1.0 }, /* 3 */
    {-1.0,  1.0,  1.0 }, /* 4 */
    { 1.0,  1.0,  1.0 }, /* 5 */
    { 1.0,  1.0, -1.0 }, /* 6 */
    {-1.0,  1.0, -1.0 }  /* 7 */
};
//kauha
static int kauha_polys[][4] =
{
    { 3, 0, 1, 1 },	//kolmio
    { 7, 4, 5, 5 },	//toinen kolmio
    { 3, 7, 4, 0 },	//pienin sivu
    { 3, 1, 5, 4 }	//pohja 
};
//kaivurin peruspalikka
static int box_polys[][4] =
{
    { 2, 3, 7, 6 },
    { 3, 2, 1, 0 },
    { 0, 4, 7, 3 },
    { 1, 2, 6, 5 },
    { 4, 5, 6, 7 },
    { 0, 1, 5, 4 }
};
//kaivurin peruspalikka
static float box_normals[][3] =
{
    { 0.0, 0.0,  -1.0 },
    { 0.0, -1.0,  0.0 },
    { -1.0, 0.0, 0.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0,  1.0,  0.0 },
    { 0.0,  0.0,  1.0 }	
};

//maan pinta
void draw_plane(void) {
    glColor3f(0.0, 0.6, 0.0);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-100.0,-100.0, 0.0);
    glVertex3f( -100.0, 100.0, 0.0);
    glVertex3f( 100.0, 100.0, 0.0);
    glVertex3f(100.0, -100.0, 0.0);
    glEnd();
}

//***kameran maaritykset***//
static float eyex = 5.0, eyey = 0.6, eyez = 1.0;
static float refx = -1.0, refy = 0.0, refz = 0.0;
static float katselu_kulma = 65.0, near = 1.0, far = 40.0;

//***valaistus***//
GLint valot[VALOJA] = {GL_FALSE, GL_FALSE, GL_FALSE};	//valaistuksien on/off-muuttujat
GLfloat light_ambient_a[] = { 0.5, 0.5, 0.5, 1.0 };	//ymparistovalaistusarvo
//kohdistettu valo
GLfloat light_position_x[] = { 1.0, 0.0, 0.0, 0.0 }; 	// valonlahteen paikka
GLfloat light_diffuse_x[] = { 0.7, 0.7, 0.0, 1.0 };	//Yhdestä suunnasta, mutta tasaisesti heijastuvan valon RGBA-arvo.
GLfloat light_specular_x[] = { 1.0, 1.0, 0.0, 1.0 };	//Yhdestä sunnasta tulevan ja yhteen suuntaan heijastuvan valon RGBA-arvo.
//pistevalo
GLfloat light_diffuse_s[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular_s[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_direction_s[] = { 1.0, -1.0, 0.0 };	//Spottivalon paikka x,y,z-koordinaatistossa. 
GLfloat light_position_s[] = { -0.95, 7.8, -7.0, 1.0 };

//***materiaalin ominaisuudet***//
GLfloat mat_ambient[] = { 0.9, 0.9, 0.9, 1.0 };		//objektista heijastuva hajaantuva vari
GLfloat mat_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };		//objektin vari
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };	//tiettyyn suuntaan heijastuva kirkas valo
GLfloat mat_shininess[] = { 40.0 };	//kirkkauden voimakkuus
static int materiaali = GL_TRUE;
static int cull_face = GL_FALSE;
static int z_bufferi = GL_TRUE;
//sumu
static int sumu = GL_FALSE;
GLfloat sumun_vari[4] = { 0.7, 0.7, 0.7, 1.0 };

//***2D-rasterifunktiot***//
static void alusta_tausta() {
    int i, j;

    for (i = 0; i < pikseleita; i++)
      for (j = 0; j < pikseleita; j++) 
        kuva[i][j] = tausta;
}
static void aseta_pikseli(int x, int y, int leveys, float varikoodi) {
    int i, j;
    for (i=-leveys/2; i <= leveys/2; i++)
        for (j=-leveys/2; j <= leveys/2; j++) {
		//jos menee yli
		if((x+i) < 0)
			x = -i;
		else if((x+i) >= pikseleita)
			x = pikseleita - i - 1;
		if((y+j) < 0)
			y = -j;
		else if((y+j) >= pikseleita)
			y = pikseleita - j - 1;
		kuva[x+i][y+j] = varikoodi;
	}
}
static void viiva(int x0, int y0, int x1, int y1, int leveys, int varikoodi) {
    // piirtaa viivan annettujen paatepisteiden valille. HUOM x0 <= x1 ja y0 <= y0
    int dx, dy, x, y, p;
    float kulmakerroin;

    dx = x1 - x0;
    dy = y1 - y0;
    kulmakerroin = (GLfloat)dy / dx;
   if (kulmakerroin < 1.00) {
	x = x0; y = y0;
   } else {
	//vaihdetaan y ja x paittain -> peilaus
	dy = dx;
	dx = y1 - y0;
	x1 = y0 + dx;
	y1 = x0 + dy;
	x = y0; y = x0;
   }
	p = 2 * dy - dx;  // on muuttuja joka kertoo, onko seuraava piste samalla tasolla (vierekkäiset y-arvot samat) 
	while (x < x1) { 
		x = x + 1;
		if (p < 0)	//mikäli p<0 seuraavan pisteen y-arvo on sama
		p = p + 2 * dy; 
		else {
		y = y + 1;
		p = p + 2 * dy - 2 * dx;  //dy:n ja dx:n suhde vaikuttaa, onko seuraava piste samalla tasolla (y-arvot)
		}
	if (kulmakerroin < 1.00)
		aseta_pikseli(x, y, leveys, varikoodi);
	else
		aseta_pikseli(y, x, leveys, varikoodi);
	}
}
static void kaaripisteet(int keski_x, int keski_y, int x, int y, int valimatka, int leveys, int varikoodi) {
	// merkitsee vasemman ja oikean kaaren pisteen (x ja y) sateen ja valimatkan mukaan seka ne 7 muuta symmetria-pistetta    
    aseta_pikseli(keski_x + x + valimatka/2, keski_y + y, leveys, varikoodi);  	//ylin oikea
    aseta_pikseli(keski_x - x - valimatka/2, keski_y + y, leveys, varikoodi);	//ylin vasen
    aseta_pikseli(keski_x - x - valimatka/2, keski_y - y, leveys, varikoodi);	//alin vasen
    aseta_pikseli(keski_x + x + valimatka/2, keski_y - y, leveys, varikoodi);	//alin oikea
    aseta_pikseli(keski_x + y + valimatka/2, keski_y + x, leveys, varikoodi);	//oikein ylä
    aseta_pikseli(keski_x - y - valimatka/2, keski_y + x, leveys, varikoodi);	//vasein ylä
    aseta_pikseli(keski_x - y - valimatka/2, keski_y - x, leveys, varikoodi);	//vasein ala
    aseta_pikseli(keski_x + y + valimatka/2, keski_y - x, leveys, varikoodi);	//oikein ala
}
static void kaaret(int keski_x, int keski_y, int sade, int valimatka, int leveys, int varikoodi) {
	//vasen ja oikea kaari annetulla sateella annetun valimatkan paahan
	int x, y, p;

	x = 0; y = sade;
	kaaripisteet(keski_x, keski_y, x, y, valimatka, leveys, varikoodi);
	p = 5 / 4  - sade;
	while (x < y) {
		x = x + 1;
		if (p < 0)	//jos p<0 vierekkäisten pisteiden y-arvo samat
			p = p + 2 * x + 1;
        	else {
			y = y - 1;
			p = p + 2 * x + 1 - 2 * y;    
        	}
    		kaaripisteet(keski_x, keski_y, x, y, valimatka, leveys, varikoodi);	// merkitsee 8 pistetta symmetriaa kayttaen
    	}	
}
static void boundary_fill_4(int x, int y, int varikoodi)	// alueen taytto rekursiivisesti
{
    if(x<pikseleita && x>-1 && y<pikseleita && y>-1)
	if ((kuva[x][y] != rataviivat) && (kuva[x][y] != varikoodi)){
		kuva[x][y] = varikoodi;
		boundary_fill_4(x+1, y, varikoodi);	// vierekkaisten neljan pisteen varjays
		boundary_fill_4(x-1, y, varikoodi);
		boundary_fill_4(x, y+1, varikoodi);
		boundary_fill_4(x, y-1, varikoodi);
    	}
}
static void rajat() {
	int i;
	//rajataan nurmikentta
	viiva(kentan_kp_x - kentan_pituus/2, 
		kentan_kp_y + kentan_leveys/2,
		kentan_kp_x + kentan_pituus/2,
		kentan_kp_y + kentan_leveys/2,
		raja_leveys, rataviivat);
	viiva(kentan_kp_x + kentan_pituus/2, 
		kentan_kp_y - kentan_leveys/2,
		kentan_kp_x + kentan_pituus/2,
		kentan_kp_y + kentan_leveys/2,
		raja_leveys, rataviivat);
	viiva(kentan_kp_x - kentan_pituus/2, 
		kentan_kp_y - kentan_leveys/2,
		kentan_kp_x + kentan_pituus/2,
		kentan_kp_y - kentan_leveys/2,
		raja_leveys, rataviivat);
	viiva(kentan_kp_x - kentan_pituus/2, 
		kentan_kp_y - kentan_leveys/2,
		kentan_kp_x - kentan_pituus/2,
		kentan_kp_y + kentan_leveys/2,
		raja_leveys, rataviivat);
	kaaret(kentan_kp_x, kentan_kp_y, kentan_leveys/2, kentan_pituus-1, raja_leveys, rataviivat);	//sisaradan sisempi viiva
	//merkitaan loput 8 rataa
	for(i=1; i<=ratoja; i++) {
		viiva(kentan_kp_x - kentan_pituus/2, 
			kentan_kp_y + kentan_leveys/2 + i * rata_leveys,
			kentan_kp_x + kentan_pituus/2,
			kentan_kp_y + kentan_leveys/2 + i * rata_leveys,
			raja_leveys, rataviivat);
		viiva(kentan_kp_x - kentan_pituus/2, 
			kentan_kp_y - (kentan_leveys/2 + i * rata_leveys),
			kentan_kp_x + kentan_pituus/2,
			kentan_kp_y - (kentan_leveys/2 + i * rata_leveys),
			raja_leveys, rataviivat);
		kaaret(kentan_kp_x, kentan_kp_y, kentan_leveys/2 + i * rata_leveys, kentan_pituus-1, raja_leveys, rataviivat);
	}
	//maaliviiva
	viiva(kentan_kp_x + kentan_pituus/2, 
		kentan_kp_y - kentan_leveys/2 - 8 * rata_leveys,
		kentan_kp_x + kentan_pituus/2,
		kentan_kp_y - kentan_leveys/2,
		raja_leveys, rataviivat);
	//varjaysta
	boundary_fill_4(kentan_kp_x, kentan_kp_y, viherio);
	boundary_fill_4(kentan_kp_x + kentan_pituus/2 + rata_leveys, kentan_kp_y, juoksurata);
	boundary_fill_4(kentan_kp_x - (kentan_pituus/2 + rata_leveys), kentan_kp_y, juoksurata);
	for(i=0; i<ratoja; i++)
		boundary_fill_4(kentan_kp_x, kentan_kp_y + kentan_leveys/2 + rata_leveys/2 + i*rata_leveys, juoksurata);
}

//***kaivurin kasaamisfunktiot***//
static void draw_box(void)	//peruspalikka kaivurin kasaamiseen
{
    int i, j;
    float b;

    for (i = 0; i < 6; i++) {
	b = (float)i / 5.0;
	glColor3f(0.7, 0.0, b);
	glBegin(GL_POLYGON);
	glNormal3fv(box_normals[i]);
	for (j = 0; j < 4; j++)
	  glVertex3fv(box_coords[box_polys[i][j]]);
	glEnd();
    }
}
static void draw_kauha(void)	//kaivurin kauhan malli
{
    int i;
    //ovat kolmioita
    glBegin(GL_POLYGON);
    for (i = 0; i < 3; i++)
	glVertex3fv(box_coords[kauha_polys[0][i]]);
    glEnd();
    glBegin(GL_POLYGON);
    for (i = 0; i < 3; i++)
	glVertex3fv(box_coords[kauha_polys[1][i]]);
    glEnd();
    //ovat nelikulmioita
    glBegin(GL_POLYGON);
    for (i = 0; i < 4; i++)
	glVertex3fv(box_coords[kauha_polys[2][i]]);
    glEnd();
    glBegin(GL_POLYGON);
    for (i = 0; i < 4; i++)
	glVertex3fv(box_coords[kauha_polys[3][i]]);
    glEnd();
}
//***kaivurin animaatiofunktiot***//
static void kauha_alkutilaan(void);
static void nosta_kuorma(void) {
	//nostaa ja siirtaa kuorman
	int kierra = 0;
	if(kulma_varsi_y>-45) kulma_varsi_y -= tyonopeus*0.01;
	if(kulma_varsi_a<145) kulma_varsi_a += tyonopeus*0.01;
	if(kulma_varsi_y<(-45+tyonopeus*0.01)) kierra=1;
	if(kierra)
		if(kiertyma<90) kiertyma += tyonopeus*0.01;
		else glutIdleFunc(kauha_alkutilaan);
	glutPostRedisplay();
}
static void ota_kuorma(void) {
	//laskee ylemman varren niin etta kauha koskettaa maata
	int kauha = 0;
	if(kulma_varsi_y<0) kulma_varsi_y += tyonopeus*0.01;
	if(kulma_varsi_a<120) kulma_varsi_a += tyonopeus*0.01;
	if(kulma_varsi_a>90) kauha=1;
	if(kauha)
		if(kulma_kauha<130) kulma_kauha += tyonopeus*0.01;
		else glutIdleFunc(nosta_kuorma);
	glutPostRedisplay();
}
static void laske_varsi_y(void) {
	//laskee ylemman varren niin etta kauha koskettaa maata
	if(kulma_varsi_y<-12) kulma_varsi_y += tyonopeus*0.01;
	else glutIdleFunc(ota_kuorma);
	glutPostRedisplay();
}
static void kauha_alkutilaan(void) {
	//liikutellaan kauha alkutilaan, jossa voidaan ottaa kuorma kauhaan
	int kesken = 0;
	if(kulma_varsi_y>-45) {
		kesken = 1;
		kulma_varsi_y -= tyonopeus*0.01;
	}
	if(kulma_varsi_a>(60 + (tyonopeus*0.01))) {
		kesken = 1;
		kulma_varsi_a -= tyonopeus*0.01;
	} else if(kulma_varsi_a<(60 - (tyonopeus*0.01))) {
		kesken = 1;
		kulma_varsi_a += tyonopeus*0.01;
	}
	if(kulma_kauha>(53 + (tyonopeus*0.01))) {
		kesken = 1;
		kulma_kauha -= tyonopeus*0.01;
	} else if(kulma_kauha<(53 - (tyonopeus*0.01))) {
		kesken = 1;
		kulma_kauha += tyonopeus*0.01;
	}
	if(kesken ==0)
		if(kiertyma<180.0 && kiertyma>(tyonopeus*0.01)) {
			kesken = 1;
			kiertyma -= tyonopeus*0.01;
		} else if(kiertyma>=180.0 && kiertyma<(-tyonopeus*0.01)) {
			kesken = 1;
			kiertyma += tyonopeus*0.01;
		} else glutIdleFunc(laske_varsi_y);	//seuraava vaihe
	glutPostRedisplay();
}

//***valaistus***//
void set_valo(int valo){
    int i;
    valot[valo] = !valot[valo];
    for (i = 0; i < VALOJA; i++) {
	if (valot[i] == GL_TRUE) {
	    glEnable(GL_LIGHT0 + i); /*esim. GL_LIGHT0 + 2 = GL_LIGHT2*/
	} else {
	    glDisable(GL_LIGHT0 + i);
	}
    }
}

//***lista toiminnoista***//
static void listaa_toiminnot() {
        printf("\n\n\n\n\t***Kameran saataminen***");
        printf("\n\tKameran paikka:");
        printf("\n\t\t\tx: %f, A:+, a:-", eyex);
        printf("\n\t\t\ty: %f, S:+, s:-", eyey);
        printf("\n\t\t\tz: %f, D:+, d:-", eyez);
        printf("\n\tKameran suunta:");
        printf("\n\t\t\tx: %f, Z:+, z:-", refx);
        printf("\n\t\t\ty: %f, X:+, x:-", refy);
        printf("\n\t\t\tz: %f, C:+, c:-", refz);
        printf("\n\tKamera kuvaamaan kaivuria: F5-F8");
        printf("\n\t\t\tEdesta: F5");
        printf("\n\t\t\tTakaa: F6");
        printf("\n\t\t\tOikealta: F7");
        printf("\n\t\t\tVasemmalta: F8");
        printf("\n\tProjektion saataminen:");
        printf("\n\t\t\tLaajuus (kulma): %f, V:+, v:-", katselu_kulma);
        printf("\n\t\t\tNear %f, B:+, b:-", near);
        printf("\n\t\t\tFar %f, N:+, n:-", far);
        
        printf("\n\n\t***Kaivurin hallitseminen***");
        printf("\n\tKaivurin siirtaminen: nuolinappaimet, asema: %fx,%fy, suunta: %f", sijainti_x, sijainti_y, suuntakulma);
        printf("\n\tVarren ja kauhan hallitseminen:");
        printf("\n\t\t\tRungon ylaosan kiertyma: %f, T:+, t:-", kiertyma);
        printf("\n\t\t\tYlavarren asento (kulma): %f, R:+, r:-", kulma_varsi_y);
        printf("\n\t\t\tAlavarren asento (kulma): %f, E:+, e:-", kulma_varsi_a);
        printf("\n\t\t\tKauhan asento (kulma): %f, W:+, w:-", kulma_kauha);  
        printf("\n\tAutomattinen kaivaminen: paalle: F1, pois: F2");
        printf("\n\tTyonopeus: %f, F3:+, F4:-", tyonopeus);
        
        printf("\n\n\t***Valaistuksen, sumun ja materiaalin hallitseminen***");
        printf("\n\t\t\tYmparistovalaistus: 0-nappain, "); if(valot[0]) printf("paalla"); else printf("pois");
        printf("\n\t\t\tKohdevalaistus: 1-nappain, "); if(valot[1]) printf("paalla"); else printf("pois");
        printf("\n\t\t\tSpottivalo: 2-nappain, "); if(valot[2]) printf("paalla"); else printf("pois");
        printf("\n\t\t\tCull face: F9-nappain, "); if(cull_face) printf("paalla"); else printf("pois");
        printf("\n\t\t\tMateriaalin ominaisuudet: F10-nappain, "); if(materiaali) printf("paalla"); else printf("pois");
        printf("\n\t\t\tZ-bufferi: F11-nappain, "); if(z_bufferi) printf("paalla"); else printf("pois");
        printf("\n\t\t\tSumu: F12-nappain, "); if(sumu) printf("paalla"); else printf("pois");

        printf("\n\n\t***Valikko: hiiren oikea nappain.***");
	
	printf("\n\n\t***Ohjelman sulkeminen Q-nappaimesta***");
}

//***naytto***//
static void display(void) {
	int i, j;
	if(mode == KaksiD_1) {
		glDisable(GL_LIGHTING);
		glClear(GL_COLOR_BUFFER_BIT);
    		glViewport(0, 0, width/2, height);	// vasen puolisko ->
		glPushMatrix();
		glScalef(1.0, 0.5, 1.0);		// -> joudutaan skaalaamaan, jotta muoto sailyy
		glCallList(1);
		glPopMatrix();
		glViewport(width/2, height/2, width/2, height/2);	// leveys-korkeus suhde sailyy
		glCallList(1);
		glViewport(width/2, 0, width/2, height/2);
		glPushMatrix();
		glTranslatef(40, 20, 0.0);
		glCallList(2);
		glPopMatrix();
		glFlush();
	} else if(mode == KaksiD_2) {
		glDisable(GL_LIGHTING);
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		glScalef(0.5, 0.5, 1.0);
		glTranslatef(width/6, height/10, 0.0);
		glRotatef(90.0, 0.0, 0.0, 1.0);
		glCallList(1);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(width/4.5, 0.0, 0.0);
		glRotatef(90.0, 0.0, 0.0, 1.0);
		glCallList(1);
		glPopMatrix();
		glFlush();
	} else if(mode == kaivuri) {
		glEnable(GL_LIGHTING);
		//materiaalin pinnan ominaisuudet
		if (cull_face) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		} else
			glDisable(GL_CULL_FACE);
		if(materiaali)
			glEnable(GL_COLOR_MATERIAL);
		else
			glDisable(GL_COLOR_MATERIAL);
		//sumu
		if (sumu)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);

		glLoadIdentity();
		gluLookAt(eyex, eyey, eyez, refx, refy, refz, 0.0, 0.0, 1.0);

		if(z_bufferi){
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
		} else {
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
		}

		glLightfv(GL_LIGHT2, GL_POSITION, light_position_x);	//x-suunnasta tulevan valon paikka

		//halli
		GLfloat solmut1[12] = {0.0,0.0,0.0,0.0,0.0,0.0,1.0,1.0,1.0,1.0,1.0,1.0};
		GLfloat solmut2[4] = {0.0,0.0,1.0,1.0};
		GLfloat solmut3[6] = {0.0,0.0,0.0,1.0,1.0,1.0};
		GLfloat solmut4[8] = {0.0,0.0,0.0,0.0,1.0,1.0,1.0,1.0};
		glColor3f(1.0, 1.0, 1.0);
		gluBeginSurface(Nurb_halli);
		gluNurbsSurface(Nurb_halli, 
			12, solmut1,
			4, solmut2,
			2*3,
			3,
			&halli_cps1[0][0][0], 
			6, 2,
			GL_MAP2_VERTEX_3);
		gluEndSurface(Nurb_halli);
		glColor3f(0.0, 0.0, 1.0);
		gluBeginSurface(Nurb_halli);
		gluNurbsSurface(Nurb_halli, 
			6, solmut3,
			8, solmut4,
			4*3,
			3,
			&halli_cps2[0][0][0], 
			3, 4,
			GL_MAP2_VERTEX_3);
		gluEndSurface(Nurb_halli);

		glCallList(3);	//aita
		
		glPushMatrix();
		glTranslatef(eyex, eyey, 0.0);	//maa on pyorea eika littana -> reunalta ei voi pudota
		glCallList(4);	//maan pinta
		glPopMatrix();
		
		//***kaivuri***
		//lopullinen sijainti ja asento
		glTranslatef(sijainti_x, sijainti_y, 0.0);
		glTranslatef(tela_sx/2, 0.0, 0.0);	//...takaisin alkuperaiseen asemaansa
    		glRotatef(suuntakulma, 0.0, 0.0, 1.0);
		glTranslatef(-tela_sx/2, 0.0, 0.0);	//pyoritetaan keskipisteen ympari...
		//telat
		glTranslatef(tela_sx/2, 0.0, tela_sz/2);
	 	glPushMatrix();
		glTranslatef(0.0, runko1_sy/2 - tela_sy/2, 0.0);
		glScalef(tela_sx, tela_sy, tela_sz);
		glCallList(5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0, tela_sy/2 - runko1_sy/2, 0.0);
		glScalef(tela_sx, tela_sy, tela_sz);
		glCallList(5);
		glPopMatrix();
		//runko1
		glTranslatef(0.0, 0.0, tela_sz/2 + runko1_sz/2);
		glPushMatrix();
		glScalef(runko1_sx, runko1_sy, runko1_sz);
		glCallList(5);
		glPopMatrix();
		//runko2
		glTranslatef(0.0, 0.0, runko1_sz/2 + runko2_sz/2);
		glRotatef(kiertyma, 0.0, 0.0, 1.0);
		glPushMatrix();
		glScalef(runko2_sx, runko2_sy, runko2_sz);
		glCallList(5);
		glPopMatrix();
		//runko3
		glTranslatef( -(runko3_sx/2 - runko1_sx/2), 0.0, runko3_sz/2 + runko2_sz/2);
		glPushMatrix();
		glScalef(runko3_sx, runko3_sy, runko3_sz);
		glCallList(5);
		glPopMatrix();
		//koppi ja pakoputki
		glPushMatrix();
		glTranslatef(0.0, 0.0, runko3_sz/2);
		//1. koppi
		glPushMatrix();
		glTranslatef(runko3_sx/2 - koppi_sx/2, runko3_sy/2 - koppi_sy/2, koppi_sz/2);
		glScalef(koppi_sx, koppi_sy, koppi_sz);
		glCallList(5);
		glPopMatrix();
		//2. pakoputki
		glPushMatrix();
		glTranslatef(-runko3_sx/4, 0.0, pakoputki_sz/2);
		glScalef(pakoputki_sx, pakoputki_sy, pakoputki_sz);
		glCallList(5);
		glPopMatrix();
		glPopMatrix();
		//ylempi varsi
		glTranslatef(varsi_y_sx/2, 0.0, 0.0);
		glRotatef(kulma_varsi_y, 0.0, 1.0, 0.0);
		glPushMatrix();
		glTranslatef(varsi_y_sx/2, 0.0, 0.0);		//juuri on origossa
		glScalef(varsi_y_sx, varsi_y_sy, varsi_y_sz);
		glCallList(5);
		glPopMatrix();
		//alempi varsi
		glTranslatef(varsi_y_sx, 0.0, 0.0);
		glRotatef(kulma_varsi_a, 0.0, 1.0, 0.0);
		glPushMatrix();
		glTranslatef(varsi_a_sx/2, 0.0, 0.0);		//juuri on origossa
		glScalef(varsi_a_sx, varsi_a_sy, varsi_a_sz);
		glCallList(5);
		glPopMatrix();
		//kauha
		glTranslatef(varsi_a_sx, 0.0, 0.0);
		glRotatef(kulma_kauha, 0.0, 1.0, 0.0);
		glTranslatef(kauha_sx/2, 0.0, 0.0);
		glScalef(kauha_sx, kauha_sy, kauha_sz);
		glCallList(6);

		glutSwapBuffers();
	}
}
static void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w; height = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(mode <= KaksiD_2) {
	if (w <= h)
	gluOrtho2D(-1.0, (GLfloat)pikseleita,
			-1.0 * (GLfloat)h / (GLfloat)w,
			(GLfloat)pikseleita * (GLfloat)h / (GLfloat)w);
	else
	gluOrtho2D(-1.0 * (GLfloat)w / (GLfloat)h,
			(GLfloat)pikseleita * (GLfloat)w / (GLfloat)h,
			-1.0, (GLfloat)pikseleita);
    } else if(mode == kaivuri)
	gluPerspective(katselu_kulma, (GLfloat)w / (GLfloat)h, near, far);
    glMatrixMode(GL_MODELVIEW);
}

void CALLBACK nurbsError(GLenum errorCode)
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf (stderr, "Nurbs Error: %s\n", estring);
   exit (0);
}

//***menufunktiot***//
static void menu_moodi(int value) {
    mode = value;
    if(mode <= KaksiD_2) {
    	glutSetWindowTitle("Yleisurheilukentta");
    } else if(mode == kaivuri) {
        glutSetWindowTitle("Kaivinkone");
    }
    reshape(width, height);
}
static void menu_ymparisto_valaistus(int value) {
    switch (value) {
      case 0:
	if(!valot[0]) set_valo(0);
	break;
      case 1:
	if(valot[0]) set_valo(0);
	break;
    }
    glutPostRedisplay();
}
static void menu_kohdevalo(int value) {
    switch (value) {
      case 0:
	if(!valot[1]) set_valo(1);
	break;
      case 1:
	if(valot[1]) set_valo(1);
	break;
    }
    glutPostRedisplay();
}
static void menu_pistevalo(int value) {
    switch (value) {
      case 0:
	valot[2] = GL_TRUE;
	break;
      case 1:
	valot[2] = GL_FALSE;
	break;
    }
    glutPostRedisplay();
}
static void menu_animaatio(int value) {
    switch (value) {
      case 0:
	glutIdleFunc(kauha_alkutilaan);
	break;
      case 1:
	glutIdleFunc(NULL);
	break;
    }
}
static void menu_tyonopeus(int value) {
	tyonopeus = value*5;
}
static void menu_kuvakulma(int value) {
    switch (value) {
      case 0:	//edesta
	eyex = sijainti_x + 5*cos(PI*suuntakulma/180);
        eyey = sijainti_y + 5*sin(PI*suuntakulma/180);
        eyez = 1.0;
        refx = sijainti_x;
        refy = sijainti_y;
        refz = 1.0;
      case 1:	//takaa
	eyex = sijainti_x - 5*cos(PI*suuntakulma/180);
        eyey = sijainti_y - 5*sin(PI*suuntakulma/180);
        eyez = 1.0;
        refx = sijainti_x;
        refy = sijainti_y;
        refz = 1.0;
        break;
      case 2:	//oikealta
	eyex = sijainti_x + 5*sin(PI*suuntakulma/180);
        eyey = sijainti_y - 5*cos(PI*suuntakulma/180);
        eyez = 1.0;
        refx = sijainti_x;
        refy = sijainti_y;
        refz = 1.0;
        break;
     case 3:	//vasemmalta
	eyex = sijainti_x - 5*sin(PI*suuntakulma/180);
        eyey = sijainti_y + 5*cos(PI*suuntakulma/180);
        eyez = 1.0;
        refx = sijainti_x;
        refy = sijainti_y;
        refz = 1.0;
        break;
    }
}
static void menu_cull_face(int value) {
    switch (value) {
      case 0:
	cull_face = GL_TRUE;
	break;
      case 1:
	cull_face = GL_FALSE;
	break;
    }
    glutPostRedisplay();
}
static void menu_materiaali(int value) {
    switch (value) {
      case 0:
	materiaali = GL_TRUE;
	break;
      case 1:
	materiaali = GL_FALSE;
	break;
    }
    glutPostRedisplay();
}
static void menu_z_bufferi(int value) {
    switch (value) {
      case 0:
	z_bufferi = GL_TRUE;
	break;
      case 1:
	z_bufferi = GL_FALSE;
	break;
    }
    glutPostRedisplay();
}
static void menu_sumu(int value) {
    switch (value) {
      case 0:
	sumu = GL_TRUE;
	break;
      case 1:
	sumu = GL_FALSE;
	break;
    }
    glutPostRedisplay();
}
static void menu_main(int value) {
    switch (value) {
      case 99:
	exit(0);
    }
}

//***alustukset***//
static void init() {
    int i, j, varikoodi;

    alusta_tausta();
    rajat();

    glNewList(1, GL_COMPILE);
    glBegin(GL_POINTS);

    for (i=0; i<pikseleita; i++)
        for (j=0; j<pikseleita; j++) {
            varikoodi = kuva[i][j];
            glColor3fv(varit[varikoodi]);
            glVertex2i(i, j);
        }
    glEnd();
    glEndList();

    //jalkapallokentta
    glNewList(2, GL_COMPILE);
    glColor3f(0.0, 0.7, 0.0);	//himmeampi vihrea
    glBegin(GL_POLYGON);
    for (i=0; i<4; i++)
      glVertex2fv(poly_jpk[i]);
    glEnd();
    //rajat
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    for (i=0; i<8; i++)
      glVertex2fv(poly_jpk_rajat[i]);
    glEnd();
    glEndList();

    Nurb_halli = gluNewNurbsRenderer();
    gluNurbsProperty(Nurb_halli, GLU_SAMPLING_TOLERANCE, 10.0);
    gluNurbsProperty(Nurb_halli, GLU_DISPLAY_MODE, GLU_FILL);

    //aita: kuvioinnin maarittely
    Image *TextureImage = (Image *) malloc(sizeof(Image));	//kuviointipointteri
    glEnable(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    ImageLoad(AITA_TEX, TextureImage); 
    //pinnan maarittely
    glNewList(3, GL_COMPILE);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);	//kuviointi
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
    glNormal3fv(aita_normaali[0]);
    for (i = 0; i < 4; i++) {
	//Mapping between the texture coordinates and the object coordinates
   	glTexCoord2fv(aita_t[i]);
	glVertex3fv(aita_coords[i]);
    }
    glEnd();
    glEndList();
    free(TextureImage->data);
    free(TextureImage );

    //maanpinta
    glNewList(4, GL_COMPILE);
    draw_plane();
    glEndList();

     //perusboxi kaivurin kasaamiseen
    glNewList(5, GL_COMPILE);
    glScalef(0.5, 0.5, 0.5);	//vaikuttaa palikoiden siirtoihin kappaleessa
    draw_box();
    glEndList();
    glNewList(6, GL_COMPILE);	//kauha
    glColor3f(0.2, 0.2, 0.2);
    glScalef(0.5, 0.5, 0.5);
    draw_kauha();
    glEndList();

    //***valaistus***
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_a);	//ymparistovalo
    //x-suunnasta tuleva valo (position on maaritetty display:ssa)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse_x);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular_x);
    //spottivalo
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_direction_s);	//Spottivalon paikka x,y,z-koordinaatistossa.
    glLightfv(GL_LIGHT2, GL_POSITION, light_position_s);	//valonlahteen paikka
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse_s);		//Yhdestä suunnasta, mutta tasaisesti heijastuvan valon RGBA-arvo.
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular_s);	//Yhdestä sunnasta tulevan ja yhteen suuntaan heijastuvan valon RGBA-arvo.
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 50.0);		//Spottivalosta leikattava osa (kulma)
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 3.0); 	//Spottivalon hajaantumista ilmaiseva luku. 
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.5);	//Valon heikkenemiskerroin.
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.01);	//Valon heikkenemiskerroin.
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0);	//Valon heikkenemiskerroin.
    //sumu
    glFogi (GL_FOG_MODE, GL_EXP);
    glFogfv (GL_FOG_COLOR, sumun_vari);
    glFogf (GL_FOG_DENSITY, 0.05);
    glHint (GL_FOG_HINT, GL_DONT_CARE);
    glFogf (GL_FOG_START, 20);
    glFogf (GL_FOG_END, 40);
    
    //materiaalin ominaisuudet (värit, heijastukset ym)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    //***valikko***//
    int id_moodi, id_ymparisto_valaistus, id_kohdevalo, id_pistevalo, id_valaistus, id_animaatio, id_tyonopeus, id_kuvakulma, id_kaivinkone, id_cull_face;
    int id_materiaali, id_z_bufferi, id_sumu, id_pinta;
    //1.0moodi
    id_moodi = glutCreateMenu(menu_moodi);
    glutAddMenuEntry("2D-1", 1);
    glutAddMenuEntry("2D-2", 2);
    glutAddMenuEntry("3D", 3);
    //2.1ymparistovalaistus
    id_ymparisto_valaistus = glutCreateMenu(menu_ymparisto_valaistus);
    glutAddMenuEntry("Paalle", 0);
    glutAddMenuEntry("Pois paalta", 1);
    //2.2kohdevalo
    id_kohdevalo = glutCreateMenu(menu_kohdevalo);
    glutAddMenuEntry("Paalle", 0);
    glutAddMenuEntry("Pois paalta", 1);
    //2.3pistevalo
    id_pistevalo = glutCreateMenu(menu_pistevalo);
    glutAddMenuEntry("Paalle", 0);
    glutAddMenuEntry("Pois paalta", 1);
    //2.0valaistus
    id_valaistus = glutCreateMenu(NULL);
    glutAddSubMenu("Ymparistovalaistus", id_ymparisto_valaistus);
    glutAddSubMenu("Kohdevalo", id_kohdevalo);
    glutAddSubMenu("Pistevalo", id_pistevalo);
    //3.1animaatio
    id_animaatio = glutCreateMenu(menu_animaatio);
    glutAddMenuEntry("Paalle", 0);
    glutAddMenuEntry("Pois paalta", 1);
    //3.2tyonopeus
    id_tyonopeus = glutCreateMenu(menu_tyonopeus);
    glutAddMenuEntry("1", 1);
    glutAddMenuEntry("2", 2);
    glutAddMenuEntry("3", 3);
    glutAddMenuEntry("4", 4);
    glutAddMenuEntry("5", 5);
    //3.3kuvakulma
    id_kuvakulma = glutCreateMenu(menu_kuvakulma);
    glutAddMenuEntry("Edesta", 0);
    glutAddMenuEntry("Takaa", 1);
    glutAddMenuEntry("Oikealta", 2);
    glutAddMenuEntry("Vasemmalta", 3);
    //3.0kaivuri
    id_kaivinkone = glutCreateMenu(NULL);
    glutAddSubMenu("Animaatio", id_animaatio);
    glutAddSubMenu("Tyonopeus", id_tyonopeus);
    glutAddSubMenu("Kuvakulma", id_kuvakulma);
    //4.1cull_face
    id_cull_face = glutCreateMenu(menu_cull_face);
    glutAddMenuEntry("Paalle", 0);
    glutAddMenuEntry("Pois paalta", 1);
    //4.2materiaalin ominaisuudet
    id_materiaali = glutCreateMenu(menu_materiaali);
    glutAddMenuEntry("Paalle", 0);
    glutAddMenuEntry("Pois paalta", 1);
    //4.3z-bufferi
    id_z_bufferi = glutCreateMenu(menu_z_bufferi);
    glutAddMenuEntry("Paalle", 0);
    glutAddMenuEntry("Pois paalta", 1);
    //4.4sumu
    id_sumu = glutCreateMenu(menu_sumu);
    glutAddMenuEntry("Paalle", 0);
    glutAddMenuEntry("Pois paalta", 1);
    //4.0pinnan ominaisuudet ja sumu
    id_pinta = glutCreateMenu(NULL);
    glutAddSubMenu("Cull face", id_cull_face);
    glutAddSubMenu("Materiaalin ominaisuudet", id_materiaali);
    glutAddSubMenu("Z-bufferi", id_z_bufferi);
    glutAddSubMenu("Sumu", id_sumu);
    //paavalikko
    glutCreateMenu(menu_main);
    glutAddSubMenu("Moodi", id_moodi);
    glutAddSubMenu("Valaistus", id_valaistus);
    glutAddSubMenu("Kaivurin hallinta", id_kaivinkone);	
    glutAddSubMenu("Pinnan ominaisuudet ja sumu", id_pinta);
    glutAddMenuEntry("Sulje ohjelma", 99);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    listaa_toiminnot();     //nappaintoiminnot
}

//***nappaintapahtumat***//
static void keyboard(unsigned char key, int x, int y) {
    if(mode == kaivuri) {
            switch (key) {
              	//kamera ja katselupiste
        	case 'a': eyex -= 0.5; break;
        	case 'A': eyex += 0.5; break;
        	case 's': eyey -= 0.5; break;
        	case 'S': eyey += 0.5; break;
        	case 'd': eyez -= 0.5; break;
        	case 'D': eyez += 0.5; break;
        	case 'z': refx -= 0.05; break;
        	case 'Z': refx += 0.05; break;
        	case 'x': refy -= 0.05; break;
        	case 'X': refy += 0.05; break;
        	case 'c': refz -= 0.05; break;
        	case 'C': refz += 0.05; break;
        	case 'v': katselu_kulma -= 0.1; break;
        	case 'V': katselu_kulma += 0.1; break;
        	case 'b': near -= 0.1; break;
        	case 'B': near += 0.1; break;
        	case 'n': far -= 0.1; break;
        	case 'N': far += 0.1; break;
        	//kaivuri
        	case 't': kiertyma -=  tyonopeus; break;
        	case 'T': kiertyma += tyonopeus; break;
        	case 'r': kulma_varsi_y -= tyonopeus; break;
        	case 'R': kulma_varsi_y += tyonopeus; break;
        	case 'e': kulma_varsi_a -= tyonopeus; break;
        	case 'E': kulma_varsi_a += tyonopeus; break;
        	case 'w': kulma_kauha -= tyonopeus; break;
        	case 'W': kulma_kauha += tyonopeus; break;
        	//valaistus
        	case '0': case '1': case '2': set_valo(key - '0'); break;
		//ohjelman sulkeminen
		case 'q': exit(0); break; case 'Q': exit(0); break;
            }
        	//rajat
        	if(kiertyma>360.0) kiertyma=0.0;
        	else if(kiertyma<0.0) kiertyma=360.0;
        	if(kulma_varsi_y>10.0) kulma_varsi_y=10.0;
        	else if(kulma_varsi_y<-45.0) kulma_varsi_y=-45.0;
        	if(kulma_varsi_a>150.0) kulma_varsi_a=150.0;
        	else if(kulma_varsi_a<0.0) kulma_varsi_a=0.0;
        	if(kulma_kauha>150.0) kulma_kauha=150.0;
        	else if(kulma_kauha<0.0) kulma_kauha=0.0;
        
        	reshape(width, height);	//paivittaa perspektiivin    	
		listaa_toiminnot();     //nappaintoiminnot
        	glutPostRedisplay();
        }
}
//erikoisnappaimet
static void special(int key, int x, int y) {
        if(mode == kaivuri) {
            switch (key) {
        	case GLUT_KEY_F1:
        	glutIdleFunc(kauha_alkutilaan);
        	break;
              case GLUT_KEY_F2:
        	glutIdleFunc(NULL);
        	break;
              case GLUT_KEY_F3:
        	if(tyonopeus<50) tyonopeus += 5;
        	printf("\nTyonopeus: %d", tyonopeus/5);
        	break;
             case GLUT_KEY_F4:
        	if(tyonopeus>9) tyonopeus -= 5;
        	printf("\nTyonopeus: %d", tyonopeus/5);
        	break;
		//F5-F8 kameran paikka kaivuriin pain
            case GLUT_KEY_F5:	//edesta
        	eyex = sijainti_x + 5*cos(PI*suuntakulma/180);
        	eyey = sijainti_y + 5*sin(PI*suuntakulma/180);
        	eyez = 1.0;
        	refx = sijainti_x;
        	refy = sijainti_y;
        	refz = 1.0;
        	break;
            case GLUT_KEY_F6:	//takaa
        	eyex = sijainti_x - 5*cos(PI*suuntakulma/180);
        	eyey = sijainti_y - 5*sin(PI*suuntakulma/180);
        	eyez = 1.0;
        	refx = sijainti_x;
        	refy = sijainti_y;
        	refz = 1.0;
        	break;
            case GLUT_KEY_F7:	//oikealta
        	eyex = sijainti_x + 5*sin(PI*suuntakulma/180);
        	eyey = sijainti_y - 5*cos(PI*suuntakulma/180);
        	eyez = 1.0;
        	refx = sijainti_x;
        	refy = sijainti_y;
        	refz = 1.0;
        	break;
            case GLUT_KEY_F8:	//vasemmalta
        	eyex = sijainti_x - 5*sin(PI*suuntakulma/180);
        	eyey = sijainti_y + 5*cos(PI*suuntakulma/180);
        	eyez = 1.0;
        	refx = sijainti_x;
        	refy = sijainti_y;
        	refz = 1.0;
        	break;
            case GLUT_KEY_F9:	//cull_face
        	cull_face = !cull_face;
        	break;
            case GLUT_KEY_F10:	//materiaalin ominaisuudet
        	materiaali = !materiaali;
        	break;
            case GLUT_KEY_F11:
        	z_bufferi = !z_bufferi;
        	break;
            case GLUT_KEY_F12:
        	sumu = !sumu;
        	break;
            case GLUT_KEY_UP:
        	sijainti_x += tyonopeus*0.01*cos(PI*suuntakulma/180);
        	sijainti_y += tyonopeus*0.01*sin(PI*suuntakulma/180);
        	break;
            case GLUT_KEY_DOWN:
        	sijainti_x -= tyonopeus*0.01*cos(PI*suuntakulma/180);
        	sijainti_y -= tyonopeus*0.01*sin(PI*suuntakulma/180);
        	break;
            case GLUT_KEY_LEFT:
        	suuntakulma += 0.1*tyonopeus;
        	break;
            case GLUT_KEY_RIGHT:
        	suuntakulma -= 0.1*tyonopeus;
        	break;
            }
            glutPostRedisplay();
	    listaa_toiminnot();     //nappaintoiminnot
        }
}
//paaohjelma
int main(int argc, char * * argv)
{
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1500, 1500);
    glutCreateWindow("Yleisurheilukentta");
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    init();
    glutMainLoop();
    return 0;
}
