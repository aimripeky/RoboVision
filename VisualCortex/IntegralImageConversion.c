#include "IntegralImageConversion.h"
#include "Precalculations.h"
#include "VisCortexTimer.h"



/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    This code is dated , and I have written a more compact version here
    https://github.com/AmmarkoV/RGBDAcquisition/blob/master/tools/ImageOperations/summedAreaTables.c

    if someone is looking on how to calculate integral images ( a.k.a. summed area tables ) this is better
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/




/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        START OF OLDER HISTOGRAM PATCH CODE !
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

inline void GenerateCompressHistogramOfImage_AddUpHorizontal(unsigned char * input_img,unsigned short * output_img,unsigned int block_x,unsigned int block_y)
{
  register unsigned char *startblock_px,*image_px;
  unsigned short * store_px;
  unsigned short * store_r,*store_g,*store_b;
  register unsigned char *r,*g,*b;

  unsigned int tot_r=0,tot_g=0,tot_b=0;
  unsigned int first_r=0,first_g=0,first_b=0;
  unsigned int x=0,xlim,y=0;

  unsigned int res_x=metrics[RESOLUTION_X];
  unsigned int vertical_position=0,vertical_step=metrics[RESOLUTION_X_3_BYTE];
  while (y<metrics[RESOLUTION_Y])
    {

      image_px=input_img+vertical_position;
      startblock_px=input_img+vertical_position;
      store_px=output_img+vertical_position;


      vertical_position+=vertical_step;

      tot_r=0,tot_g=0,tot_b=0; // THE LINE IS INITIALLY EMPTY

      r=startblock_px++;
      first_r = *r;
      g=startblock_px++;
      first_g = *g;
      b=startblock_px++;
      first_b = *b;
      // WE PREPARE OUR BUFFER WE SUM UP THE FIRST PATCH_X R , G AND B VALUES!
      x=0;
      xlim=block_x;
      while (x<xlim)
        {
          r=image_px++;
          tot_r+=*r;
          g=image_px++;
          tot_g+=*g;
          b=image_px++;
          tot_b+=*b;
          ++x;
        }


      // WE SET THE END , THE X END OF THE CURRENT Y LINE OF THE  IMAGE
      xlim=res_x; // metrics[RESOLUTION_X]
      // HORIZONTAL MAIN LOOP THAT ADDS UP EVERYTHING
      while (x<xlim)
        {
          //STORE THE HORIZONTAL SUM AT THE FIRST PIXEL
          store_r=store_px++;
          *store_r = (unsigned short) tot_r;
          store_g=store_px++;
          *store_g = (unsigned short) tot_g;
          store_b=store_px++;
          *store_b = (unsigned short) tot_b;

          //SUBTRACT FIRST PIXEL FROM NEW SUM
          tot_r-=first_r;
          tot_g-=first_g;
          tot_b-=first_b;

          //NEW FIRST PIXEL IS LAST AT IMAGE_PX
          r=startblock_px++;
          first_r = *r;
          g=startblock_px++;
          first_g = *g;
          b=startblock_px++;
          first_b = *b;

          r=image_px++;
          tot_r+=*r;
          g=image_px++;
          tot_g+=*g;
          b=image_px++;
          tot_b+=*b;
          ++x;
        }
      ++y;
    }
  return;
}

inline void GenerateCompressHistogramOfImage_AddUpVertical(unsigned short * output_img,unsigned int block_x,unsigned int block_y)
{

  unsigned short *start_store_px,*store_px;
  unsigned short *r,*g,*b,*store_r,*store_g,*store_b;

  unsigned int tot_r=0,tot_g=0,tot_b=0;
  unsigned int first_tot_r=0,first_tot_g=0,first_tot_b=0;
  unsigned int xlim,ylim;
  unsigned int total_area=block_x*block_y;
  unsigned int vertical_step=metrics[RESOLUTION_X_3_BYTE];


  /* THELOUME GIA KATHE ENA APO TA PIXELS STON X AKSONA NA ATHROISOUME TO PRECALCULATION
     TO OPOIO EXEI DIMIOURGITHEI KATHETA!
  */
  unsigned int x=0,y=0;
  xlim=metrics[RESOLUTION_X];
  while ( x<xlim )
    {
      y=0; // EFOSON EIMASTE STO NEO X PIXEL KSEKINAME APO Y=0

      store_px=output_img+(x*3); // Y=0 OPOTE O POINTER PREPEI NA DEIXNEI POINTER + X * 3
      // ||
      // \/ DEIXNEI PANTA STIN METAVLITI POU EXEI TO first_tot_r ,first_tot_g ,first_tot_b
      start_store_px =output_img+(x*3); // Y=0 OPOTE O POINTER PREPEI NA DEIXNEI POINTER + X * 3

      /*
       APOTHIKEYOUME STA R G B k meta sta FIRST_TOT_R , FIRST_TOT_G, FIRST_TOT_B
       TO PIXEL X,0 TO OPOIO PERIEXEI TA ATHROISMATA APO X,0 EWS X+PATCH_X,0
      */
      r=start_store_px;
      first_tot_r = *r;
      g=start_store_px+1;
      first_tot_g = *g;
      b=start_store_px+2;
      first_tot_b = *b;

      tot_r=0,tot_g=0,tot_b=0;
      /*
       GEMIZOUME OLO TO PATCH_Y ATHROISMA ETSI WSTE NA MPOROUME NA GLYTWSOUME TIS EXTRA PRAKSEIS :)
      */
      ylim=metrics[VERTICAL_BUFFER];
      while (y<ylim)
        {
          ++y;
          store_r=store_px;
          tot_r+=*store_r;
          store_g=store_px+1;
          tot_g+=*store_g;
          store_b=store_px+2;
          tot_b+=*store_b;
          store_px+=vertical_step;
        }
      /*
        ANTI NA ATHROIZOUME OLO TO PATCH KATHE FORA AFAIROUME TO FIRST_TOT_X k PROSTHETOUME TO LAST_TOT_X kathe fora!
      */
      ylim=metrics[RESOLUTION_Y];
      while (y<ylim)
        {
          /*
            APOTHIKEYOUME TO APOTELESMA TOU ISTOGRAMMATOS
            STA PRWTA 3 SHORTS STA OPOIA DEIXNEI TO START_STORE_PX
          */
          r=start_store_px;
          *r=(unsigned short) (tot_r/total_area);
          g=start_store_px+1;
          *g=(unsigned short) (tot_g/total_area);
          b=start_store_px+2;
          *b=(unsigned short) (tot_b/total_area);
          start_store_px+=vertical_step;
          /*
            AFAIROUME TIN TIMI TWN TRIWN PROTWN METRISEWN APO TO ATHROISMA
            GIA NA PROSTHESOUME MONO TIN TELEYTAIA KAI NA GLYTWSOUME POLLES PRAKSEIS!
          */
          tot_r-=first_tot_r;
          tot_g-=first_tot_g;
          tot_b-=first_tot_b;

          /*
            H NEA PRWTI TIMI EINAI AYTI STIN OPOIA DEIXNEI TWRA POIA O DEIKTIS
          */
          r=start_store_px;
          first_tot_r = *r;
          g=start_store_px+1;
          first_tot_g = *g;
          b=start_store_px+2;
          first_tot_b = *b;

          /*
            PROSTHETOUME TIN NEA TELEYTAIA TIMI STO ATHROISMA KAI H DIADIKASIA SYNEXIZETAI
          */
          store_r=store_px;
          tot_r+=*store_r;
          store_g=store_px+1;
          tot_g+=*store_g;
          store_b=store_px+2;
          tot_b+=*store_b;
          store_px+=vertical_step;

          ++y;
        }
      ++x;
    }
  return;
}


void GenerateCompressHistogramOfImage(unsigned char * input_img,unsigned short * output_img,unsigned int block_x,unsigned int block_y)
{
  /*
       WE WANT EVERY PIXEL OF OUTPUT_IMG TO CONTAIN THE MEDIAN VALUE OF ALL THE R, G AND B PIXELS OF THE BLOCK
       For example to find out the median R value of the block starting at x,y and ending at x+block_x,y+block_y
       you basically read the value output_img[x][y] without any additions to speed up the Disparity Depth map
       functions :)

       THE CODE IS BROKEN IN TWO PARTS TO HELP CLARITY :)
  */
  GenerateCompressHistogramOfImage_AddUpHorizontal(input_img,output_img,block_x,block_y);
  /*
     AFTER THIS CODE EVERY PIXEL CONTAINS THE R , G AND B SUM OF THE NEXT BLOCK_X PIXELS
    ON THE HORIZONTAL AXIS

    NEXT THING TO DO SUM THEM UP VERTICALLY AND DIVIDE THEM BY BLOCK_X*BLOCK_Y
    AND STORE THE RESULT AS A PRECALCULATION! :)
  */
  GenerateCompressHistogramOfImage_AddUpVertical(output_img,block_x,block_y);
  /*
     WE SHOULD HAVE ACHIEVED OUR GOAL :)
  */
}




/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        END OF OLDER HISTOGRAM PATCH CODE !
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


inline int MakePatchFitInsideImage(int *x,int *y ,int *width,int *height)
{
  if ( *x >= metrics[RESOLUTION_X] )
    {
      return 0;
    }
  if ( *y >= metrics[RESOLUTION_Y] )
    {
      return 0;
    }
  if ( *x + *width >= metrics[RESOLUTION_X] )
    {
      *width  = metrics[RESOLUTION_X] - *x;
    }
  if ( *y + *height >= metrics[RESOLUTION_Y])
    {
      *height = metrics[RESOLUTION_Y] - *y;
    }
  return 1;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   START OF GET FUNCTIONS FOR COMPRESSED AND NOT IMAGES
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


unsigned int inline GetRegisterPatchSum(int comp_register, unsigned int x , unsigned int y,unsigned int width , unsigned int  height,unsigned int depth)
{
  if (!VideoRegisterRequestIsOk(comp_register,metrics[RESOLUTION_X],metrics[RESOLUTION_Y],3))
    {
      return 0;
    }
  unsigned int counted_edges=0;
  unsigned int x_c=x ,  y_c=y;
  unsigned char *in_ptr_start = video_register[comp_register].pixels;
  register BYTE *px;
  register BYTE *stopx;

  while (y_c<y+height)
    {
      if ( depth == 1 )
        {
          px= (BYTE *) in_ptr_start + MEMPLACE1(x_c,y_c,metrics[RESOLUTION_X]); //( y_c * ( metrics[RESOLUTION_X] ) + x_c ) ;
        }
      else if ( depth == 3 )
        {
          px= (BYTE *) in_ptr_start + MEMPLACE3(x_c,y_c,metrics[RESOLUTION_X]);
        }
      else
        {
          return 0;
        }

      stopx=px+width;
      while (px<stopx)
        {
          counted_edges+=(*px);
          px+=depth;
        }

      ++y_c;
    }
  return counted_edges;
}


unsigned int inline GetRegisterPatchPresenceSum(int comp_register, unsigned int x , unsigned int y,unsigned int width , unsigned int  height,unsigned int depth,unsigned int threshold)
{
  if (!VideoRegisterRequestIsOk(comp_register,metrics[RESOLUTION_X],metrics[RESOLUTION_Y],3)) { return 0; }
  unsigned int counted_edges=0;
  unsigned int x_c=x ,  y_c=y , end_y = y + height;
  unsigned char *in_ptr_start = video_register[comp_register].pixels;
  register BYTE *px;
  register BYTE *stopx;

  while (y_c<end_y)
    {
      if ( depth == 1 ) { px= (BYTE *) in_ptr_start + MEMPLACE1(x_c,y_c,metrics[RESOLUTION_X]);  stopx=px+width;     } else
      if ( depth == 3 ) { px= (BYTE *) in_ptr_start + MEMPLACE3(x_c,y_c,metrics[RESOLUTION_X]); stopx=px+(width*3); } else
                        { return 0; }


      while (px<stopx)
        {
          if  (*px>=threshold) { ++counted_edges; } else
          if ( depth == 3 )
           {
               if  (*(px+1)>=threshold) { ++counted_edges; } else
               if  (*(px+2)>=threshold) { ++counted_edges; }
           }

          px+=depth;
        }

      ++y_c;
    }
  return counted_edges;
}

unsigned int GetRegisterPatchSum1Byte(int comp_register, unsigned int x , unsigned int y,unsigned int width , unsigned int  height)
{
  return GetRegisterPatchSum(comp_register,x,y,width,height,1);
}
unsigned int inline GetRegisterPatchSum3Byte(int comp_register, unsigned int x , unsigned int y,unsigned int width , unsigned int  height)
{
  return GetRegisterPatchSum(comp_register,x,y,width,height,3);
}




unsigned int inline GetCompressedRegisterPatchSum1Byte(int comp_register,int x,int y,int width,int height)
{
  if ( (comp_register<0) || (comp_register>=EXTRA_LARGE_REGISTERS_COUNT) )
           {
              fprintf(stderr,"GetCompressedRegisterPatchSum1Byte called with register %u ( %u  registers exist )",comp_register,EXTRA_LARGE_REGISTERS_COUNT);
              return 0;
           }
  if (!MakePatchFitInsideImage(&x,&y,&width,&height)) { return 0; }
/*
  unsigned int ptr_up_left = metrics[RESOLUTION_X]*y+x;
  unsigned int ptr_up_right = metrics[RESOLUTION_X]*y+(x+width);
  unsigned int ptr_down_left = metrics[RESOLUTION_X]*(y+height)+x;
  unsigned int ptr_down_right = metrics[RESOLUTION_X]*(y+height)+(x+width);
  */

  register unsigned int * ptr_up_left = xl_video_register[comp_register].pixels + metrics[RESOLUTION_X]*y+x;
  register unsigned int * ptr_up_right = ptr_up_left + width;
  register unsigned int * ptr_down_left = ptr_up_left + metrics[RESOLUTION_X]*(height);
  register unsigned int * ptr_down_right =ptr_down_left + width;

  return (  *ptr_up_left
          + *ptr_down_right
          - *ptr_up_right
          - *ptr_down_left );

}

int GetCompressedRegisterPatchSum3Byte(int comp_register,int x,int y,int width,int height)
{
    fprintf(stderr,"Copy paste and change 1 to 3 :P \n");
    return 0;

}

unsigned int GetCompressedRegisterPatchSum(int comp_register,int x,int y,int width,int height)
{
    if (xl_video_register[comp_register].depth==1) { return GetCompressedRegisterPatchSum1Byte(comp_register,x,y,width,height); }
     else
    if (xl_video_register[comp_register].depth==3) { return GetCompressedRegisterPatchSum3Byte(comp_register,x,y,width,height); }
    return 0;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   END OF GET FUNCTIONS FOR COMPRESSED AND NOT IMAGES
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

unsigned int CompressRegister1Byte(struct VideoRegister *   input,struct ExtraLargeVideoRegister *   output)
{

  if ( ( input==0 ) || ( output==0 ) ) { fprintf(stderr,"Compress Register called with incorrect registers\n"); return 0; }
//  if (!VideoRegisterRequestIsOk(input,metrics[RESOLUTION_X],metrics[RESOLUTION_Y],3)) { return 0; }

//  if (!ExtraLargeVideoRegisterRequestIsOk(output,metrics[RESOLUTION_X],metrics[RESOLUTION_Y],3)) { return 0; }
  //This code will add up all the pixels to every other pixel , in order to speed up access
  //Patch procedures when each pixel must be added to the others..!
  if ( !ThisIsA1ByteRegister(input) ) { fprintf(stderr,"CompressRegister1Byte called with 3byte image\n"); return 0; }

 StartTimer(COMPRESS_IMAGE_DELAY); // STATISTICS KEEPER FOR HYPERVISOR | START

  ClearExtraLargeVideoRegister(output);
  unsigned char *in_ptr_start=input->pixels,*in_ptr=in_ptr_start;
  unsigned int *out_ptr_start=output->pixels,*out_ptr=out_ptr_start,*out_ptr_adj=out_ptr_start;
  output->depth = 1;

  unsigned int x=0,y=0;
  while (y<metrics[RESOLUTION_Y])
    {
      x = metrics[RESOLUTION_X];

      in_ptr += metrics[RESOLUTION_X];
      out_ptr += metrics[RESOLUTION_X];

      while (x>0)
        {
          --in_ptr;
          --out_ptr;
          --x;

          *out_ptr += *in_ptr;
          out_ptr_adj = out_ptr-1;
          if (x>0) *out_ptr_adj = *out_ptr;
        }


      in_ptr += metrics[RESOLUTION_X];
      out_ptr += metrics[RESOLUTION_X];

      ++y;
    }

  x=0;
  y=0;
  unsigned int last_val=0;
  while (x<metrics[RESOLUTION_X])
    {
      y = metrics[RESOLUTION_Y];
      out_ptr=out_ptr_start+metrics[RESOLUTION_MEMORY_LIMIT_1BYTE]+x; // You have subtract at least metrics[RESOLUTION_X]; to make it a usable pointer
      last_val = *out_ptr;

      while (y>0)
        {
          out_ptr -= metrics[RESOLUTION_X];
          --y;
          *out_ptr += last_val;
          last_val=*out_ptr;
        }
      ++x;
    }

 EndTimer(COMPRESS_IMAGE_DELAY); // STATISTICS KEEPER FOR HYPERVISOR | START

// THIS IS WRONG -> MarkRegistersAsSynced(input,output);
   output->time = input->time; // Instead of MarkRegistersAsSynced

   return 1;
}




unsigned int CompressRegister3Byte(struct VideoRegister *   input,struct ExtraLargeVideoRegister  *   output)
{

  if ( ( input==0 ) || ( output==0 ) ) { fprintf(stderr,"Compress Register called with incorrect registers\n"); return 0; }
  fprintf(stderr,"CompressRegister3Byte NOT implemented ( the implementation is of CompressRegister1Byte ) !\n");
  return 0;
/*
  if (!VideoRegisterRequestIsOk(input,metrics[RESOLUTION_X],metrics[RESOLUTION_Y],3)) { return 0; }
  if (!ExtraLargeVideoRegisterRequestIsOk(output,metrics[RESOLUTION_X],metrics[RESOLUTION_Y],3)) { return 0; }
  //This code will add up all the pixels to every other pixel , in order to speed up access
  //Patch procedures when each pixel must be added to the others..!
  if ( video_register[input].depth != 3 )
    {
      fprintf(stderr,"CompressRegister3Byte called with 1byte image\n");
      return 0;
    }

 StartTimer(COMPRESS_IMAGE_DELAY); // STATISTICS KEEPER FOR HYPERVISOR | START

  ClearExtraLargeVideoRegister(output);
  unsigned char *in_ptr_start=video_register[input].pixels,*in_ptr=in_ptr_start;
  unsigned int *out_ptr_start=xl_video_register[output].pixels,*out_ptr=out_ptr_start,*out_ptr_adj=out_ptr_start;
  xl_video_register[output].depth = 1;

  unsigned int x=0,y=0;
  while (y<metrics[RESOLUTION_Y])
    {
      x = metrics[RESOLUTION_X];

      in_ptr += metrics[RESOLUTION_X_3_BYTE];
      out_ptr += metrics[RESOLUTION_X_3_BYTE];

      while (x>0)
        {
          // R CHANNEL
          --in_ptr;
          --out_ptr;

          *out_ptr += *in_ptr;

          // G CHANNEL
          --in_ptr;
          --out_ptr;

          *out_ptr += *in_ptr;

          // B CHANNEL
          --in_ptr;
          --out_ptr;

          *out_ptr += *in_ptr;

          --x;
          out_ptr_adj = out_ptr-1;
          if (x>0) *out_ptr_adj = *out_ptr;
        }


      in_ptr += metrics[RESOLUTION_X_3_BYTE];
      out_ptr += metrics[RESOLUTION_X_3_BYTE];

      ++y;
    }

  x=0;
  y=0;
  unsigned int last_val=0;
  while (x<metrics[RESOLUTION_X])
    {
      y = metrics[RESOLUTION_Y];
      out_ptr=out_ptr_start+metrics[RESOLUTION_MEMORY_LIMIT_1BYTE]+x; // You have subtract at least metrics[RESOLUTION_X]; to make it a usable pointer
      last_val = *out_ptr;

      while (y>0)
        {
          out_ptr -= metrics[RESOLUTION_X];
          --y;
          *out_ptr += last_val;
          last_val=*out_ptr;
        }
      ++x;
    }

 EndTimer(COMPRESS_IMAGE_DELAY); // STATISTICS KEEPER FOR HYPERVISOR | START
 MarkRegistersAsSynced(input,output);
*/
  return 1;
}






unsigned int CompressRegister(struct VideoRegister *  input,struct ExtraLargeVideoRegister  *  output)
{
    if ( ( input==0 ) || ( output==0 ) ) { fprintf(stderr,"Compress Register called with incorrect registers\n"); return 0; }

    if (input->depth==1) { return CompressRegister1Byte(input,output); }
     else
    if (input->depth==3) { return CompressRegister3Byte(input,output); }
    return 0;
}






/*

TODO ADD AN MMX/SSE IMPLEMENTATION HERE !!

void integral_image( const uint8_t* in, int32_t* out, int w, int h ) {
      int32_t* out_top = out;
      const uint8_t* line_end = in + w;
      const uint8_t* in_end = in + w*h;
      int32_t line_sum = 0;
      for( ; in != line_end; in++, out++ ) {
        line_sum += *in;
        *out = line_sum;
      }
      for( ; in != in_end; ) {
        int32_t line_sum = 0;
        const uint8_t* line_end = in + w;
        for( ; in != line_end; in++, out++, out_top++ ) {
          line_sum += *in;
          *out = *out_top + line_sum;
        }
      }
    }
*/







/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   START OF TESTER
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


int TestIntegralImaging()
{
  return 1;

/*

  fprintf(stderr,"TestIntegralImaging starting ( %u x %u ) \n",metrics[RESOLUTION_X],metrics[RESOLUTION_Y]);
  if ( !VideoRegisterRequestIsOk(GENERAL_4,1,1,1 ) )
    {
      return 0;
    }
  unsigned char *start_px = video_register[GENERAL_4].pixels;
  unsigned char *px = video_register[GENERAL_4].pixels;
  video_register[GENERAL_4].depth=1;
  unsigned int image_size = metrics[RESOLUTION_X] * metrics[RESOLUTION_Y];
  while ( px < start_px+image_size)
    {
      *px=1;
      ++px;
    }

  unsigned int * opx = xl_video_register[GENERAL_XLARGE_1].pixels;
  unsigned int * start_opx = opx;
  xl_video_register[GENERAL_XLARGE_1].depth=1;
  while ( opx < start_opx+image_size*3)
    {
      *opx=1;
      ++opx;
    }
  //ALL OF GENERAL_4 IS now filled with ones :)
  fprintf(stderr,"CompressRegister1Byte called \n");
  CompressRegister1Byte(GENERAL_4,GENERAL_XLARGE_1);
  fprintf(stderr,"CompressRegister1Byte survived\n");

  PrintRegister("GENERAL_4_register.html",GENERAL_4);
  PrintExtraLargeRegister("GENERAL_XLARGE_1_register.html",GENERAL_XLARGE_1);
  fprintf(stderr,"TestIntegralImaging : \n");
  fprintf(stderr,"%u vs %u ",GetCompressedRegisterPatchSum1Byte(GENERAL_XLARGE_1,1,1,2,2),GetRegisterPatchSum1Byte(GENERAL_4,1,1,2,2));


  fprintf(stderr,"TestIntegralImaging survived\n");
  return 1;*/
}

