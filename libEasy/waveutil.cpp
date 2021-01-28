/*
* File Name: waveutils.cpp
* For wave file manipulation utility
*/

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include <math.h>


#include "easy.h"

#ifndef SIZEOF_WAVEFORMATEX
#define SIZEOF_WAVEFORMATEX(pwfx)   ((WAVE_FORMAT_PCM==(pwfx)->wFormatTag)?sizeof(PCMWAVEFORMAT):(sizeof(WAVEFORMATEX)+(pwfx)->cbSize))
#endif

static WAVEFORMATEX* get_driver_format(HACMDRIVERID hadid, WORD wFormatTag);
static BOOL CALLBACK find_format_enum(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport);
//static ACMFORMATENUMCB CALLBACK find_format_enum(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport);
static BOOL CALLBACK find_driver_enum(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport);
//static ACMFORMATENUMCB CALLBACK find_driver_enum(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport);
static HACMDRIVERID find_driver(WORD wFormatTag);


int egGetWaveStat(char *fileName, EGWaveStat_t *stat)
{
	HMMIO          hmmioIn = NULL;					// handle to open input WAVE file
	MMCKINFO       ckInRIFF;						// chunk info. for input RIFF chunk
	MMCKINFO       ckIn;	// info. for a chunk in input file

	memset(stat, 0, sizeof(EGWaveStat_t));
	
	// Open the input file for reading using buffered I/O.
	hmmioIn = mmioOpen(fileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmioIn == NULL)
	{
		egerrno = _EG_BADFILE;
		return -1;
	}

	//Descend the input file into the 'RIFF' chunk.
	if (mmioDescend(hmmioIn, &ckInRIFF, NULL, 0) != 0)
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Make sure the input file is a WAVE file.
	if ((ckInRIFF.ckid != FOURCC_RIFF) || 
		 ckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	//assign memory to source_format
	WAVEFORMATEX *source_format = (WAVEFORMATEX*)(new BYTE[sizeof(WAVEFORMATEX)]);
	if( !source_format )
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_MEMERR;
		return -1;
	}
	
	// Read the 'fmt ' chunk data into <source_format>.
	if (mmioRead(hmmioIn, (HPSTR)source_format,
		(long) sizeof(*source_format)) != (long) sizeof(*source_format))
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	stat->waveFormat   = source_format->wFormatTag;
    stat->sampleRate   = source_format->nSamplesPerSec;
	stat->bitsPerSample = source_format->wBitsPerSample;

	

	// comes out of the input file out of the 'fmt ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

// read the data chunk

	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// get the size
	stat->size = ckIn.cksize;
	
	// comes out of the input file out of the 'data ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'RIFF ' chunk.
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	delete[] source_format;
    mmioClose(hmmioIn, 0);
	
	return 0;

}




int egConvertTo1Bit( char* source_file,char* des_file)
{

	HMMIO          hmmioIn = NULL;					// handle to open input WAVE file
	HMMIO          hmmioOut = NULL;					// handle to open output WAVE file
	MMCKINFO       ckInRIFF;						// chunk info. for input RIFF chunk
	MMCKINFO       ckOutRIFF;						// chunk info. for output RIFF chunk
	MMCKINFO       ckIn;						    // info. for a chunk in input file
	MMCKINFO       ckOut;					        // info. for a chunk in output file
	MMRESULT	   mr;						        //result of read write operations on file


	


	// Open the input file for reading using buffered I/O.
	hmmioIn = mmioOpen(source_file, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmioIn == NULL)
	{
		egerrno = _EG_BADFILE;
		return -1;
	}

	//Descend the input file into the 'RIFF' chunk.
	if (mmioDescend(hmmioIn, &ckInRIFF, NULL, 0) != 0)
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Make sure the input file is a WAVE file.
	if ((ckInRIFF.ckid != FOURCC_RIFF) || 
		 ckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	//assign memory to source_format
	WAVEFORMATEX *source_format = (WAVEFORMATEX*)(new BYTE[sizeof(WAVEFORMATEX)]);
	if( !source_format )
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
	
	// Read the 'fmt ' chunk data into <source_format>.
	if (mmioRead(hmmioIn, (HPSTR)source_format,
		(long) sizeof(*source_format)) != (long) sizeof(*source_format))
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

#if 0
	//check the format to be true speech or PCM to be same 	
/**
//[GI]
**/
#if 0	
	if(!(( (source_format->wFormatTag      == WAVE_FORMAT_PCM)||
#else
	if(!(( (source_format->wFormatTag      == WAVE_FORMAT_PCM)||(source_format->wFormatTag      == WAVE_FORMAT_GSM610) ||
#endif
		   (source_format->wFormatTag      == WAVE_FORMAT_DSPGROUP_TRUESPEECH) /*|| (source_format->wFormatTag      == 6) */ )&&
		
		(  (source_format->nAvgBytesPerSec == 8000)||
		   (source_format->nAvgBytesPerSec == 1067)   ) &&
		
		(  (source_format->nSamplesPerSec  == 8000)   ) &&   
		
		(  (source_format->wBitsPerSample  == 1)||
		   (source_format->wBitsPerSample  == 8)      )&&   
		
		(  (source_format->nChannels       == 1)      )&&
		
		(  (source_format->nBlockAlign     == 1)||
		   (source_format->nBlockAlign     == 32)  )
		   )
	  )    
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
#endif
	// comes out of the input file out of the 'fmt ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}


	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}


	// reading the data size of input file   
	DWORD   source_data_size = ckIn.cksize;
	if( !source_data_size )
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	//assign the memory to the source_dat
	HPSTR   source_data = (HPSTR)(new BYTE[source_data_size]);
	if( !source_data )
	{
		mmioClose(hmmioIn, 0);
		delete[] source_format;
		egerrno = _EG_MEMERR;
		return -1;
	}

	//read the data of saurce file in the source_data variable 
	//if fail to read then return the func.
	if(mmioRead (hmmioIn, source_data, source_data_size) != LONG(source_data_size))
	{
		mmioClose(hmmioIn, 0);
		delete[] source_data;
		delete[] source_format;
		egerrno = _EG_BADFILE;
		return -1;
	} 
	
	// comes out of the input file out of the 'data ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'RIFF ' chunk.
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) != 0)
	{
		delete[] source_format;
		delete[] source_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
	
	//close the input file
	mmioClose(hmmioIn, 0);


	// Now we locate a CODEC that supports  format tag WAVE_FORMAT_DSPGROUP_TRUESPEECH
	HACMDRIVERID hadid = find_driver(WAVE_FORMAT_DSPGROUP_TRUESPEECH );
	if (hadid == NULL)
	{
		delete[] source_data;
		delete[] source_format;
		
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
		
	}



	// get a format The driver supports with specific format tag
	WAVEFORMATEX *true_speech_format = get_driver_format(hadid,
		                                      WAVE_FORMAT_DSPGROUP_TRUESPEECH );
	if (true_speech_format == NULL)
	{
		delete[] source_data;
		delete[] source_format;
		
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}


    //get the format the driver with PCM format tag  
	WAVEFORMATEX *pcm_format = get_driver_format(hadid,WAVE_FORMAT_PCM);
	if (pcm_format == NULL)
	{
		delete[] source_data;
		delete[] source_format;
		delete[] true_speech_format;
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}


	//open the driver for conversion and get its handle in had
	HACMDRIVER had = NULL;
	mr = acmDriverOpen(&had, hadid, 0);
	if (mr)
	{
		delete[] source_data;
		delete[] source_format;
		delete[] true_speech_format;
		delete[] pcm_format;
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}

	


	//open the stream for conversion for converting from 
	//pcm to true speech format get its handle in hstr
	
	//setbthe des format as per the requirement
	WAVEFORMATEX   *des_format;
    
	HACMSTREAM hstr = NULL;

	//if the source format is pcm open the stream to convert the data to TRUE SPEECH
	if(source_format->wFormatTag == WAVE_FORMAT_PCM)
	{
		mr = acmStreamOpen(&hstr,						   //stream		
							had,						   // any driver
							pcm_format, 	               // source format  .....
							true_speech_format,    	       // destination format
							NULL,					       // no filter
							0,				      	       // no callback
							0,						       // instance data (not used)
							ACM_STREAMOPENF_NONREALTIME);  // flags
		if (mr) 
		{
			delete[] source_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;

			egerrno = _EG_NOWAVEDRIVER;
			return -1;
		}
		//set destination format to be true speech 
		des_format    = true_speech_format;

	}	//end of if the source format is PCM
	else		//when source format is true speech open the stream to 
	{			//convert the data to PCM 
	
		mr = acmStreamOpen(&hstr,						   //stream		
							had,						   // any driver
							true_speech_format,	           // source format  .....
							pcm_format,    				   // destination format
							NULL,					       // no filter
							0,				      	       // no callback
							0,						       // instance data (not used)
							ACM_STREAMOPENF_NONREALTIME);  // flags
		if (mr) 
		{

			delete[] source_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;
        	
			egerrno = _EG_NOWAVEDRIVER;
			return -1;
		}
		//set destination format to be PCM format 
		des_format    =  pcm_format;
	
	}//end of else block if source format is true speech


	
	
	// allocate a buffer for the result of the conversion
	//depending on the total time of play b'cos total time of paly 
	//for both the file would be same
	DWORD source_tot_bytes_per_sec = source_format->nAvgBytesPerSec;
	double tot_time_of_play        = (double)(source_data_size) / source_tot_bytes_per_sec;

	//calculate the des data (final) 
	DWORD des_tot_bytes_per_sec = des_format->nAvgBytesPerSec;
	double real_des_data_size = ((double)(tot_time_of_play * des_tot_bytes_per_sec));

	//get the highest integer value
	DWORD des_data_size = (DWORD) ceil(real_des_data_size);
	
	//assign the mem to final data 
	BYTE* des_data = (BYTE*) malloc(des_data_size + 500);
	
	
	#ifdef _DEBUG
	// fill the dest buffer with zeroes just so we can see if anything got 
	// converted in the debugger
	memset((void*)des_data, 0, (des_data_size));
	#endif

	// fill in the conversion info in ACMSTREAMHEADER structure to be converted
	ACMSTREAMHEADER strhdr;

	memset(&strhdr, 0, sizeof(strhdr));

	strhdr.cbStruct		    = sizeof(strhdr);
	strhdr.fdwStatus		= 0L;
	strhdr.dwUser			= 0L;
	strhdr.pbSrc			= (LPBYTE)source_data; // the source data to convert
	strhdr.cbSrcLength		= source_data_size;
	strhdr.cbSrcLengthUsed  = 0L;
	strhdr.dwSrcUser        = 0L;
	strhdr.pbDst            = (LPBYTE)des_data;
	strhdr.cbDstLength      = des_data_size;
	strhdr.cbDstLengthUsed  = 0L;
	strhdr.dwDstUser        = 0L; 

	// prep the header to convert the data 
	mr = acmStreamPrepareHeader(hstr, &strhdr, 0L); 


	
	// convert the data to the final format
	mr = acmStreamConvert(hstr, &strhdr, 0);
	if (mr)
	{
		acmStreamClose(hstr, 0);
		delete[] des_data;
		delete[] source_data;
	    delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		
		egerrno = _EG_NOWAVEDRIVER;
		return -1;

	}
	
	//unprepare the stream for future use
	mr =acmStreamUnprepareHeader(hstr, &strhdr, 0);         
	if (mr)
	{
		acmStreamClose(hstr, 0);
		delete[] des_data;
		delete[] source_data;
	    delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
	
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}
  
	//delete format and data cos this is no more needed
	delete[] source_data; 
	
	
	// close the stream
	acmStreamClose(hstr, 0);

	// Open the output file for writing using buffered I/O. 
	hmmioOut = mmioOpen((char*)des_file,
		                 NULL,
						 MMIO_ALLOCBUF | MMIO_WRITE | MMIO_CREATE|MMIO_EXCLUSIVE);
	if (hmmioOut == NULL)
	{
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;

		egerrno = _EG_WRITEERROR;
		return -1;
	}


	//Create the output file RIFF chunk of form type 'WAVE'.
	ckOutRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if (mmioCreateChunk(hmmioOut, &ckOutRIFF, MMIO_CREATERIFF) != MMSYSERR_NOERROR )
	{
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		mmioClose(hmmioOut, 0);

		egerrno = _EG_WRITEERROR;
		return -1;

	}

	
	//create the memory chunk for format
	long dw = SIZEOF_WAVEFORMATEX(des_format);
	ckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioCreateChunk(hmmioOut, &ckOut, 0) != MMSYSERR_NOERROR)
	{ 
		// cannot write file, probably
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		mmioClose(hmmioOut, 0);
		
		egerrno = _EG_WRITEERROR;
		return -1;
	}

	//write format chunk and ascend
	if (mmioWrite(hmmioOut, (HPSTR)des_format, dw) != (LONG)dw)
	{
		// cannot write file, probably
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		mmioClose(hmmioOut, 0);
		
		egerrno = _EG_WRITEERROR;
		return -1;
	}

	if (mmioAscend(hmmioOut, &ckOut, 0) == MMIOERR_CANNOTWRITE )
	{
		// cannot write file, probably
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		mmioClose(hmmioOut, 0);

		egerrno = _EG_WRITEERROR;
		return -1;

	}

	//if source format is DSP group than this fact chunk is required 
	if(des_format->wFormatTag == WAVE_FORMAT_DSPGROUP_TRUESPEECH)
	{

		//set no of samples to be playe after decompreesion
		DWORD cSamples = (DWORD)(source_format->nSamplesPerSec*(tot_time_of_play)); 
		ckOut.ckid   = mmioFOURCC('f', 'a', 'c', 't');
		ckOut.cksize = cSamples;
		if(mmioCreateChunk(hmmioOut, &ckOut, 0) != MMSYSERR_NOERROR ) 
		{
			// cannot write file, probably
			delete[] des_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;	
			mmioClose(hmmioOut, 0);
		
			egerrno = _EG_WRITEERROR;
			return -1;

		}
			
		
		if(mmioWrite(hmmioOut, (HPSTR)&cSamples, sizeof(cSamples)) !=  sizeof(cSamples))
		{
			// cannot write file, probably
			delete[] des_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;
			mmioClose(hmmioOut, 0);
			
			egerrno = _EG_WRITEERROR;
			return -1;
		}
		if(mmioAscend(hmmioOut, &ckOut, 0) == MMIOERR_CANNOTWRITE )
		{
			// cannot write file, probably
			delete[] des_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;
			mmioClose(hmmioOut, 0);

			egerrno = _EG_WRITEERROR;
			return -1;
		}
	}//end of outer if block

	//no need of these now so delete these
	delete[] source_format;
	delete[] pcm_format;       
	delete[] true_speech_format;
	
	//create the memory chunk for format
	ckOut.ckid = mmioFOURCC('d', 'a', 't', 'a');
	ckOut.cksize = des_data_size;
	if (mmioCreateChunk(hmmioOut, &ckOut,0) != MMSYSERR_NOERROR )
	{
		// cannot write file, probably
		delete[] des_data;
		mmioClose(hmmioOut, 0);

		egerrno = _EG_WRITEERROR;
		return -1;

	}


	// Create the 'data' chunk that holds the waveform samples.and ascends out of it
	if(mmioWrite (hmmioOut, 
		         (LPCTSTR)des_data,
				  des_data_size) != LONG(des_data_size))
	{

		delete[] des_data;
		mmioClose(hmmioOut, 0);
	
		egerrno = _EG_WRITEERROR;
		return -1;

	}
	//delete data buffer
	delete[] des_data;

	//comes out of file riff chunk
	if (mmioAscend(hmmioOut, &ckOut, 0) == MMIOERR_CANNOTWRITE )
	{
		mmioClose(hmmioOut, 0);
		
		egerrno = _EG_WRITEERROR;
		return -1;
	}


	//ascend out of riff chunk
	if (mmioAscend(hmmioOut, &ckOutRIFF, 0) == MMIOERR_CANNOTWRITE )
	{
		mmioClose(hmmioOut, 0);

		egerrno = _EG_WRITEERROR;
		return -1;
	}


	if (hmmioOut != NULL)
		mmioClose(hmmioOut, 0);

	//if successful returns successful
	return 0;


}//END OF CONVERT FUNCTION



// driver that supports a given id
typedef struct {
	HACMDRIVERID hadid;
	WORD wFormatTag;
} FIND_DRIVER_INFO;

// callback function for format enumeration
BOOL CALLBACK find_format_enum(HACMDRIVERID hadid,
							   LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport)
{
	FIND_DRIVER_INFO* pdi = (FIND_DRIVER_INFO*) dwInstance;
	if (pafd->dwFormatTag == (DWORD)pdi->wFormatTag) 
	{
		// found it
		pdi->hadid = hadid;
		return FALSE; // stop enumerating
	}

	return TRUE; // continue enumerating
}//END OF FIND FORMAT

// callback function for driver enumeration
BOOL CALLBACK find_driver_enum(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport)
{
 	FIND_DRIVER_INFO* pdi = (FIND_DRIVER_INFO*) dwInstance;

	// open the driver
	HACMDRIVER had = NULL;
	MMRESULT mmr = acmDriverOpen(&had, hadid, 0);
	if (mmr) {

		// some error
		return FALSE; // stop enumerating

	}

	// enumerate the formats it supports
	DWORD dwSize = 0;
	
	mmr = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &dwSize);
	
	if (dwSize < sizeof(WAVEFORMATEX)) 
			dwSize = sizeof(WAVEFORMATEX); // for MS-PCM
	
	WAVEFORMATEX* pwf = (WAVEFORMATEX*) malloc(dwSize);
	
	memset(pwf, 0, dwSize);
	
	pwf->cbSize = LOWORD(dwSize) - sizeof(WAVEFORMATEX);
	pwf->wFormatTag = pdi->wFormatTag;
	
	ACMFORMATDETAILS fd;
	
	memset(&fd, 0, sizeof(fd));
	fd.cbStruct = sizeof(fd);
	fd.pwfx = pwf;
	fd.cbwfx = dwSize;
	fd.dwFormatTag = pdi->wFormatTag;
	
	mmr = acmFormatEnum(had, &fd, (ACMFORMATENUMCB) find_format_enum, (DWORD)(VOID*)pdi, 0);
	
	free(pwf);
	
	acmDriverClose(had, 0);
	
	if (pdi->hadid || mmr) 
	{
		// found it or some error
		return FALSE; // stop enumerating
	}

	return TRUE; // continue enumeration
}  //END OF FIND DRIVER ENUM




///////////////////////////////////////////////////////////////////////////////

// locate the first driver that supports a given format tag

///////////////////////////////////////////////////////////////////////////////
HACMDRIVERID find_driver(WORD wFormatTag)
{
	FIND_DRIVER_INFO fdi;
	fdi.hadid = NULL;
	fdi.wFormatTag = wFormatTag;
	MMRESULT mmr = acmDriverEnum((ACMDRIVERENUMCB)find_driver_enum, (DWORD)(VOID*)&fdi, 0);
	if (mmr) return NULL;
	return fdi.hadid;

}//END OF FIND DRIVER



///////////////////////////////////////////////////////////////////////////////////////

// get a description of the first format supported for a given tag

/////////////////////////////////////////////////////////////////////////////////////////
WAVEFORMATEX* get_driver_format(HACMDRIVERID hadid, WORD wFormatTag)
{
	// open the driver
	HACMDRIVER had = NULL;
	MMRESULT mmr = acmDriverOpen(&had, hadid, 0);
	if (mmr) {
		return NULL;
	}

	// allocate a structure for the info
	DWORD dwSize = 0;
	mmr = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &dwSize);
	if (dwSize < sizeof(WAVEFORMATEX)) dwSize = sizeof(WAVEFORMATEX); // for MS-PCM
	WAVEFORMATEX* pwf = (WAVEFORMATEX*) malloc(dwSize);
	memset(pwf, 0, dwSize);
	pwf->cbSize = LOWORD(dwSize) - sizeof(WAVEFORMATEX);
	pwf->wFormatTag = wFormatTag;

	ACMFORMATDETAILS fd;
	memset(&fd, 0, sizeof(fd));
	fd.cbStruct = sizeof(fd);
	fd.pwfx = pwf;
	fd.cbwfx = dwSize;
	fd.dwFormatTag = wFormatTag;

	// set up a struct to control the enumeration
	FIND_DRIVER_INFO fdi;
	fdi.hadid = NULL;
	fdi.wFormatTag = wFormatTag;

	mmr = acmFormatEnum(had, &fd, (ACMFORMATENUMCB) find_format_enum, (DWORD)(VOID*)&fdi, 0);
	acmDriverClose(had, 0);
	if ((fdi.hadid == NULL) || mmr) {
		free(pwf);
		return NULL;
	}
	
	return pwf;
}//END OF GET DRIVER FORMAT






//////////////////////////////////////////////////////////////////////////////////////

//THIS FUNCTION APPEND THE DATA TO A WAVE FILE   

//FROM SECOND FILE PASSED AS ARGUMENT 

//ARGUMENTS - BASE ADDRESS OF CHAR BUFFER HAVING 

//APPEND_TO_FILE  AND APPEND_FROM_FILE


/////////////////////////////////////////////////////////////////////////////////////////

int egAppendWave(char* append_to_file,char* append_from_file)
{

	
	HMMIO          hmmioIn = NULL;					// handle to open input WAVE file
	MMCKINFO       ckInRIFF;						// chunk info. for input RIFF chunk
	MMCKINFO       ckIn;						    // info. for a chunk in input file
	

	// Open the input file for reading using buffered I/O.
	hmmioIn = mmioOpen(append_to_file, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmioIn == NULL)
	{
		egerrno = _EG_BADFILE;
		return -1;
	}

	//Descend the input file into the 'RIFF' chunk.
	if (mmioDescend(hmmioIn, &ckInRIFF, NULL, 0) != 0)
	{
//		MESSAGE("cannot descend to RIFF \n\n ");  // end-of-file, probably
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Make sure the input file is a WAVE file.
	if ((ckInRIFF.ckid != FOURCC_RIFF) || ckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
//		MESSAGE("not a wav file\n\n");  // cannot open WAVE file
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
//		MESSAGE("not descend to fmt chunk\n\n");     // no 'fmt ' chunk
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	//assign memory to source_format
	WAVEFORMATEX *source_format = (WAVEFORMATEX*)(new BYTE[sizeof(WAVEFORMATEX)]);
	if( !source_format )
	{
//		MESSAGE("could not assign memory\n\n");
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;	}
	
	// Read the 'fmt ' chunk data into <source_format>.
	if (mmioRead(hmmioIn, (HPSTR)source_format,
		(long) sizeof(*source_format)) != (long) sizeof(*source_format))
	{
//		MESSAGE("uanble to read file format\n\n");
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'fmt ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
//		MESSAGE("unable to ascend of fmt chunk input file\n\n");  
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
//		MESSAGE("no data chunk");   
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}


	// reading the data size of input file   
	DWORD   persent_data_size = ckIn.cksize;
	if( !persent_data_size )
	{
//		MESSAGE("\n Data chunk actually has no data\n\n");
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	//assign the memory to the source_dat
	HPSTR   persent_data = (HPSTR)(new BYTE[persent_data_size]);
	if( !persent_data )
	{
//		MESSAGE("memory cannot be created"); 
		mmioClose(hmmioIn, 0);
		delete[] source_format;
		egerrno = _EG_BADFILE;
		return -1;
	}

	//read the data of saurce file in the persent_data variable 
	//if fail to read then return the func.
	if(mmioRead (hmmioIn, persent_data, persent_data_size) 
		!= LONG(persent_data_size))
	{
//		MESSAGE("Failed to read file may be very big file"); 
		mmioClose(hmmioIn, 0);
		delete[] persent_data;
		delete[] source_format;
		egerrno = _EG_BADFILE;
		return -1;
	} 
	
	// comes out of the input file out of the 'data ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
//		MESSAGE("unable to ascend of data chunk input file\n\n");  
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'RIFF ' chunk.
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) != 0)
	{
//		MESSAGE("unable to ascend of RIFF chunk input file\n\n");  
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
	
	//close the input file
	mmioClose(hmmioIn, 0);
	hmmioIn = NULL;

	
	// Open the input file for reading using buffered I/O.
	hmmioIn = mmioOpen(append_from_file, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmioIn == NULL)
	{
//		MESSAGE("cannot read wav file\n\n");  // cannot open WAVE file
		delete[] source_format;
		delete[] persent_data;
		egerrno = _EG_BADFILE;
		return -1;
	}

	//Descend the input file into the 'RIFF' chunk.
	if (mmioDescend(hmmioIn, &ckInRIFF, NULL, 0) != 0)
	{
//		MESSAGE("cannot descend to RIFF \n\n ");
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Make sure the input file is a WAVE file.
	if ((ckInRIFF.ckid != FOURCC_RIFF) || ckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
//		MESSAGE("not a wav file\n\n");  
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}



	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
//		MESSAGE("not descend to fmt chunk\n\n");     // no 'fmt ' chunk
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	//assign memory to source_format
	WAVEFORMATEX *dest_format = (WAVEFORMATEX*)(new BYTE[sizeof(WAVEFORMATEX)]);
	if( !dest_format )
	{
//		MESSAGE("could not assign memory\n\n");
		mmioClose(hmmioIn, 0);
		egerrno = _EG_MEMERR;
		return -1;
	}
	
	// Read the 'fmt ' chunk data into <source_format>.
	if (mmioRead(hmmioIn, (HPSTR)dest_format,
		(long) sizeof(*dest_format)) != (long) sizeof(*dest_format))
	{
//		MESSAGE("uanble to read file format\n\n");
		delete[] dest_format;
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'fmt ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
//		MESSAGE("unable to ascend of fmt chunk input file\n\n");  
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;

	}

	//check the format of both the files to be same 	
	if( dest_format->wFormatTag      != source_format->wFormatTag     ||
		dest_format->nAvgBytesPerSec != source_format->nAvgBytesPerSec||
		dest_format->nSamplesPerSec  != source_format->nSamplesPerSec ||
		dest_format->wBitsPerSample  != source_format->wBitsPerSample ||
		dest_format->nChannels       != source_format->nChannels      ||
		dest_format->nBlockAlign     != source_format->nBlockAlign     )
	{
		
//		MESSAGE(" \n format of both the files are not same");
		delete[] source_format;
		delete[] dest_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	//delete det_format b'cos no more required
	delete[] dest_format;

	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
//		MESSAGE("no data chunk");   
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}


	// reading the data size of input file   
	DWORD   append_data_size = ckIn.cksize;
	if( !append_data_size )
	{
//		MESSAGE("\n Data chunk actually has no data\n\n");
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	//assign the memory to the source_dat
	HPSTR   data_to_append = (HPSTR)(new BYTE[append_data_size]);
	if( !persent_data )
	{
//		MESSAGE("memory cannot be created"); 
		mmioClose(hmmioIn, 0);
		delete[] source_format;
		delete[] persent_data;
		egerrno = _EG_MEMERR;
		return -1;
	}

	//read the data of saurce file in the persent_data variable 
	//if fail to read then return the func.
	if(mmioRead (hmmioIn, data_to_append, append_data_size) != LONG(append_data_size))
	{
//		MESSAGE("Failed to read file"); 
		mmioClose(hmmioIn, 0);
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		egerrno = _EG_BADFILE;
		return -1;

	} 
	
	// comes out of the input file out of the 'data ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
//		MESSAGE("unable to ascend of data chunk input file\n\n");  
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'RIFF ' chunk.
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) != 0)
	{
//		MESSAGE("unable to ascend of RIFF chunk input file\n\n");  
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
	
	//close the input file
	mmioClose(hmmioIn, 0);
	hmmioIn = NULL;	


	//Open the output file for writing using buffered I/O. 
	hmmioIn = mmioOpen(append_to_file, NULL,MMIO_ALLOCBUF | MMIO_WRITE | MMIO_CREATE|MMIO_EXCLUSIVE);
	if (hmmioIn == NULL)
	{
//		MESSAGE("cannot OPEN wav file for out put\n\n"); // can not open WAVE file
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		egerrno = _EG_WRITEERROR;
		return -1;
	}


	//Create the output file RIFF chunk of form type 'WAVE'.
	ckInRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if (mmioCreateChunk(hmmioIn, &ckInRIFF, MMIO_CREATERIFF) != MMSYSERR_NOERROR )
	{
//		MESSAGE("cannot write file riff, probably\n\n"); // cannot write file, probably
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;
	}

	
	//create the memory chunk for format
	long dw = sizeof(*source_format);
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioCreateChunk(hmmioIn, &ckIn, 0) != MMSYSERR_NOERROR)
	{
//		MESSAGE("cannot write file FORMAT CHUNK , probably\n\n"); // cannot write file, probably
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;

	}

	//write format chunk and ascend
	if (mmioWrite(hmmioIn, (HPSTR)source_format, dw) != (LONG)dw)
	{
//		MESSAGE("cannot write format chunk, probably\n\n"); // cannot write file, probably
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;	
		mmioClose(hmmioIn, 0);

		egerrno = _EG_WRITEERROR;
		return -1;
	}

	if (mmioAscend(hmmioIn, &ckIn, 0) == MMIOERR_CANNOTWRITE )
	{
//		MESSAGE("Failed to write file format unable to ascend\n\n");  // cannot write file, probably
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;
	}

	//no need of this now so delete this
	delete[] source_format;

	//create the memory chunk for data
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	ckIn.cksize = persent_data_size + append_data_size;
	if (mmioCreateChunk(hmmioIn, &ckIn,0) != MMSYSERR_NOERROR )
	{
//		MESSAGE("cannot write file create data chunk, \n\n"); // cannot write file, probably
		delete[] data_to_append;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;

	}

//	MESSAGE("writing data to output file \n\n");

	// Create the 'data' chunk that holds the waveform samples.and ascends out of it
	if(mmioWrite (hmmioIn, (LPCTSTR)persent_data, persent_data_size) != LONG(persent_data_size))
	{
//		MESSAGE("Failed to write file dat to the file \n\n"); 
		delete[] data_to_append;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;

	}
	//delete data buffer
	delete[] persent_data;

	// Create the 'data' chunk that holds the waveform samples.and ascends out of it
	if(mmioWrite (hmmioIn, (LPCTSTR)data_to_append, append_data_size) != LONG(append_data_size))
	{
//		MESSAGE("Failed to write file dat to the file \n\n"); 
		delete[] data_to_append;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;

	}
	//delete data buffer
	delete[] data_to_append;

	//comes out of file riff chunk
	if (mmioAscend(hmmioIn, &ckIn, 0) == MMIOERR_CANNOTWRITE )
	{
//		MESSAGE("Failed to write file data unable to ascend \n\n");  
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;
	}


	//ascend out of riff chunk
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) == MMIOERR_CANNOTWRITE )
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;
	}


	if (hmmioIn != NULL)
		mmioClose(hmmioIn, 0);

	//if successful returns SUCCESSFUL
	return 0;


}//END OF APPEND FUNCTION


int egAppendGSMWave(char* append_to_file,char* append_from_file)
{
	
	HMMIO          hmmioIn = NULL;					// handle to open input WAVE file
	MMCKINFO       ckInRIFF;						// chunk info. for input RIFF chunk
	MMCKINFO       ckIn;						    // info. for a chunk in input file
	

	// Open the input file for reading using buffered I/O.
	hmmioIn = mmioOpen(append_to_file, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmioIn == NULL)
	{
		egerrno = _EG_BADFILE;
		return -1;
	}

	//Descend the input file into the 'RIFF' chunk.
	if (mmioDescend(hmmioIn, &ckInRIFF, NULL, 0) != 0)
	{
//		MESSAGE("cannot descend to RIFF \n\n ");  // end-of-file, probably
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Make sure the input file is a WAVE file.
	if ((ckInRIFF.ckid != FOURCC_RIFF) || ckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
//		MESSAGE("not a wav file\n\n");  // cannot open WAVE file
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
//		MESSAGE("not descend to fmt chunk\n\n");     // no 'fmt ' chunk
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	//assign memory to source_format
	GSM610WAVEFORMAT *source_format = (GSM610WAVEFORMAT*)(new BYTE[sizeof(GSM610WAVEFORMAT)]);
	if( !source_format )
	{
//		MESSAGE("could not assign memory\n\n");
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;	}
	
	// Read the 'fmt ' chunk data into <source_format>.
	if (mmioRead(hmmioIn, (HPSTR)source_format,
		(long) sizeof(*source_format)) != (long) sizeof(*source_format))
	{
//		MESSAGE("uanble to read file format\n\n");
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'fmt ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
//		MESSAGE("unable to ascend of fmt chunk input file\n\n");  
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
//		MESSAGE("no data chunk");   
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}


	// reading the data size of input file   
	DWORD   persent_data_size = ckIn.cksize;
	if( !persent_data_size )
	{
//		MESSAGE("\n Data chunk actually has no data\n\n");
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	//assign the memory to the source_dat
	HPSTR   persent_data = (HPSTR)(new BYTE[persent_data_size]);
	if( !persent_data )
	{
//		MESSAGE("memory cannot be created"); 
		mmioClose(hmmioIn, 0);
		delete[] source_format;
		egerrno = _EG_BADFILE;
		return -1;
	}

	//read the data of saurce file in the persent_data variable 
	//if fail to read then return the func.
	if(mmioRead (hmmioIn, persent_data, persent_data_size) 
		!= LONG(persent_data_size))
	{
//		MESSAGE("Failed to read file may be very big file"); 
		mmioClose(hmmioIn, 0);
		delete[] persent_data;
		delete[] source_format;
		egerrno = _EG_BADFILE;
		return -1;
	} 
	
	// comes out of the input file out of the 'data ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
//		MESSAGE("unable to ascend of data chunk input file\n\n");  
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'RIFF ' chunk.
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) != 0)
	{
//		MESSAGE("unable to ascend of RIFF chunk input file\n\n");  
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
	
	//close the input file
	mmioClose(hmmioIn, 0);
	hmmioIn = NULL;

	
	// Open the input file for reading using buffered I/O.
	hmmioIn = mmioOpen(append_from_file, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmioIn == NULL)
	{
//		MESSAGE("cannot read wav file\n\n");  // cannot open WAVE file
		delete[] source_format;
		delete[] persent_data;
		egerrno = _EG_BADFILE;
		return -1;
	}

	//Descend the input file into the 'RIFF' chunk.
	if (mmioDescend(hmmioIn, &ckInRIFF, NULL, 0) != 0)
	{
//		MESSAGE("cannot descend to RIFF \n\n ");
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Make sure the input file is a WAVE file.
	if ((ckInRIFF.ckid != FOURCC_RIFF) || ckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
//		MESSAGE("not a wav file\n\n");  
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}



	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
//		MESSAGE("not descend to fmt chunk\n\n");     // no 'fmt ' chunk
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	//assign memory to source_format
	GSM610WAVEFORMAT *dest_format = (GSM610WAVEFORMAT*)(new BYTE[sizeof(GSM610WAVEFORMAT)]);
	if( !dest_format )
	{
//		MESSAGE("could not assign memory\n\n");
		mmioClose(hmmioIn, 0);
		egerrno = _EG_MEMERR;
		return -1;
	}
	
	// Read the 'fmt ' chunk data into <source_format>.
	if (mmioRead(hmmioIn, (HPSTR)dest_format,
		(long) sizeof(*dest_format)) != (long) sizeof(*dest_format))
	{
//		MESSAGE("uanble to read file format\n\n");
		delete[] dest_format;
		delete[] source_format;
		mmioClose(hmmioIn, 0);
	egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'fmt ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
//		MESSAGE("unable to ascend of fmt chunk input file\n\n");  
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;

	}

	//check the format of both the files to be same 	
	if( dest_format->wfx.wFormatTag      != source_format->wfx.wFormatTag     ||
		dest_format->wfx.nAvgBytesPerSec != source_format->wfx.nAvgBytesPerSec||
		dest_format->wfx.nSamplesPerSec  != source_format->wfx.nSamplesPerSec ||
		dest_format->wfx.wBitsPerSample  != source_format->wfx.wBitsPerSample ||
		dest_format->wfx.nChannels       != source_format->wfx.nChannels      ||
		dest_format->wfx.nBlockAlign     != source_format->wfx.nBlockAlign    ||
		dest_format->wSamplesPerBlock    != source_format->wSamplesPerBlock
		)
	{
		
//		MESSAGE(" \n format of both the files are not same");
		delete[] source_format;
		delete[] dest_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	//delete det_format b'cos no more required
	delete[] dest_format;

	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
//		MESSAGE("no data chunk");   
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}


	// reading the data size of input file   
	DWORD   append_data_size = ckIn.cksize;
	if( !append_data_size )
	{
//		MESSAGE("\n Data chunk actually has no data\n\n");
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	//assign the memory to the source_dat
	HPSTR   data_to_append = (HPSTR)(new BYTE[append_data_size]);
	if( !persent_data )
	{
//		MESSAGE("memory cannot be created"); 
		mmioClose(hmmioIn, 0);
		delete[] source_format;
		delete[] persent_data;
		egerrno = _EG_MEMERR;
		return -1;
	}

	//read the data of saurce file in the persent_data variable 
	//if fail to read then return the func.
	if(mmioRead (hmmioIn, data_to_append, append_data_size) != LONG(append_data_size))
	{
//		MESSAGE("Failed to read file"); 
		mmioClose(hmmioIn, 0);
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		egerrno = _EG_BADFILE;
		return -1;

	} 
	
	// comes out of the input file out of the 'data ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
//		MESSAGE("unable to ascend of data chunk input file\n\n");  
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'RIFF ' chunk.
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) != 0)
	{
//		MESSAGE("unable to ascend of RIFF chunk input file\n\n");  
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
	
	//close the input file
	mmioClose(hmmioIn, 0);
	hmmioIn = NULL;	


	//Open the output file for writing using buffered I/O. 
	hmmioIn = mmioOpen(append_to_file, NULL,MMIO_ALLOCBUF | MMIO_WRITE | MMIO_CREATE|MMIO_EXCLUSIVE);
	if (hmmioIn == NULL)
	{
//		MESSAGE("cannot OPEN wav file for out put\n\n"); // can not open WAVE file
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		egerrno = _EG_WRITEERROR;
		return -1;
	}


	//Create the output file RIFF chunk of form type 'WAVE'.
	ckInRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if (mmioCreateChunk(hmmioIn, &ckInRIFF, MMIO_CREATERIFF) != MMSYSERR_NOERROR )
	{
//		MESSAGE("cannot write file riff, probably\n\n"); // cannot write file, probably
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;
	}

	
	//create the memory chunk for format
	long dw = sizeof(*source_format);
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioCreateChunk(hmmioIn, &ckIn, 0) != MMSYSERR_NOERROR)
	{
//		MESSAGE("cannot write file FORMAT CHUNK , probably\n\n"); // cannot write file, probably
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;

	}

	//write format chunk and ascend
	if (mmioWrite(hmmioIn, (HPSTR)source_format, dw) != (LONG)dw)
	{
//		MESSAGE("cannot write format chunk, probably\n\n"); // cannot write file, probably
		delete[] data_to_append;
		delete[] source_format;
		delete[] persent_data;	
		mmioClose(hmmioIn, 0);

		egerrno = _EG_WRITEERROR;
		return -1;
	}

	if (mmioAscend(hmmioIn, &ckIn, 0) == MMIOERR_CANNOTWRITE )
	{
//		MESSAGE("Failed to write file format unable to ascend\n\n");  // cannot write file, probably
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;
	}

	//no need of this now so delete this
	delete[] source_format;

	//create the memory chunk for data
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	ckIn.cksize = persent_data_size + append_data_size;
	if (mmioCreateChunk(hmmioIn, &ckIn,0) != MMSYSERR_NOERROR )
	{
//		MESSAGE("cannot write file create data chunk, \n\n"); // cannot write file, probably
		delete[] data_to_append;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;

	}

//	MESSAGE("writing data to output file \n\n");

	// Create the 'data' chunk that holds the waveform samples.and ascends out of it
	if(mmioWrite (hmmioIn, (LPCTSTR)persent_data, persent_data_size) != LONG(persent_data_size))
	{
//		MESSAGE("Failed to write file dat to the file \n\n"); 
		delete[] data_to_append;
		delete[] persent_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;

	}
	//delete data buffer
	delete[] persent_data;

	// Create the 'data' chunk that holds the waveform samples.and ascends out of it
	if(mmioWrite (hmmioIn, (LPCTSTR)data_to_append, append_data_size) != LONG(append_data_size))
	{
//		MESSAGE("Failed to write file dat to the file \n\n"); 
		delete[] data_to_append;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;

	}
	//delete data buffer
	delete[] data_to_append;

	//comes out of file riff chunk
	if (mmioAscend(hmmioIn, &ckIn, 0) == MMIOERR_CANNOTWRITE )
	{
//		MESSAGE("Failed to write file data unable to ascend \n\n");  
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;
	}


	//ascend out of riff chunk
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) == MMIOERR_CANNOTWRITE )
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_WRITEERROR;
		return -1;
	}


	if (hmmioIn != NULL)
		mmioClose(hmmioIn, 0);

	//if successful returns SUCCESSFUL
	return 0;


}//END OF APPEND FUNCTION





int egConvertBuffGSM2PCM8K8B( unsigned char* pszGsmBuf, unsigned char * pszPcmBuf, unsigned int source_data_size )
{

	HMMIO          hmmioIn = NULL;					// handle to open input WAVE file
	HMMIO          hmmioOut = NULL;					// handle to open output WAVE file
	MMCKINFO       ckInRIFF;						// chunk info. for input RIFF chunk
	MMCKINFO       ckOutRIFF;						// chunk info. for output RIFF chunk
	MMCKINFO       ckIn;						    // info. for a chunk in input file
	MMCKINFO       ckOut;					        // info. for a chunk in output file
	MMRESULT	   mr;						        //result of read write operations on file

	WAVEFORMATEX source_format;
	
	source_format.wFormatTag=WAVE_FORMAT_GSM610; 
	source_format.nChannels=1; 
	source_format.nSamplesPerSec=8000; 
	source_format.nAvgBytesPerSec=1625; 
	source_format.nBlockAlign=65; 
	source_format.wBitsPerSample=0; 
	source_format.cbSize=sizeof(WORD); 
	
	
	// Now we locate a CODEC that supports  format tag WAVE_FORMAT_PCM
	HACMDRIVERID hadid = find_driver(WAVE_FORMAT_PCM);
	if (hadid == NULL)
	{
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	
	}

    //get the format the driver with PCM format tag  
	WAVEFORMATEX *pcm_format = get_driver_format(hadid,WAVE_FORMAT_PCM);
	if (pcm_format == NULL)
	{
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}

	HACMDRIVERID gsmhadid = find_driver(WAVE_FORMAT_GSM610);
	if (hadid == NULL)
	{
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	
	}

	// get a format The driver supports with specific format tag
	WAVEFORMATEX *gsmFormat= get_driver_format(gsmhadid,
		                                      WAVE_FORMAT_GSM610);
	if (gsmFormat == NULL)
	{
		delete[] pcm_format;

		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}



	//open the driver for conversion and get its handle in had
	HACMDRIVER had = NULL;
	mr = acmDriverOpen(&had, gsmhadid, 0);
	if (mr)
	{
		delete[] gsmFormat;
		delete[] pcm_format;
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}

	//open the stream for conversion for converting from 
	//gsm to PCM 
	
	//setbthe des format as per the requirement
	WAVEFORMATEX   *des_format;
    
	HACMSTREAM hstr = NULL;

	mr = acmStreamOpen(&hstr,						   //stream		
							NULL, //had,						   // any driver
							gsmFormat, 	               // source format  .....
							pcm_format,    	       // destination format
							NULL,					       // no filter
							0,				      	       // no callback
							0,						       // instance data (not used)
							ACM_STREAMOPENF_NONREALTIME);  // flags
		if (mr) 
		{
			delete[] pcm_format;       
			delete[] gsmFormat;

			egerrno = _EG_NOWAVEDRIVER;
			return -1;
		}
		//set destination format to be true speech 
		des_format    = pcm_format;

		
	// allocate a buffer for the result of the conversion
	//depending on the total time of play b'cos total time of paly 
	//for both the file would be same
	DWORD source_tot_bytes_per_sec = source_format.nAvgBytesPerSec;
	double tot_time_of_play        = (double)(source_data_size) / source_tot_bytes_per_sec;

	//calculate the des data (final) 
	DWORD des_tot_bytes_per_sec = des_format->nAvgBytesPerSec;
	double real_des_data_size = ((double)(tot_time_of_play * des_tot_bytes_per_sec));

	//get the highest integer value
	DWORD des_data_size = (DWORD) ceil(real_des_data_size);
	
	//assign the mem to final data 
	BYTE* des_data = (BYTE*) malloc(des_data_size + 8000);
	
	
#ifdef _DEBUG
	// fill the dest buffer with zeroes just so we can see if anything got 
	// converted in the debugger
	memset((void*)des_data, 0, (des_data_size));
#endif

	// fill in the conversion info in ACMSTREAMHEADER structure to be converted
	ACMSTREAMHEADER strhdr;

	memset(&strhdr, 0, sizeof(strhdr));

	strhdr.cbStruct		    = sizeof(strhdr);
	strhdr.fdwStatus		= 0L;
	strhdr.dwUser			= 0L;
	strhdr.pbSrc			= (LPBYTE)pszGsmBuf; // the source data to convert
	strhdr.cbSrcLength		= source_data_size;
	strhdr.cbSrcLengthUsed  = 0L;
	strhdr.dwSrcUser        = 0L;
	strhdr.pbDst            = (LPBYTE)des_data;
	strhdr.cbDstLength      = des_data_size;
	strhdr.cbDstLengthUsed  = 0L;
	strhdr.dwDstUser        = 0L; 

	// prep the header to convert the data 
	mr = acmStreamPrepareHeader(hstr, &strhdr, 0L); 


	
	// convert the data to the final format
	mr = acmStreamConvert(hstr, &strhdr, 0);
	if (mr)
	{
		acmStreamClose(hstr, 0);
		delete[] pcm_format;       
		delete[] gsmFormat;
		delete des_data;
		
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}
	
	//unprepare the stream for future use
	mr =acmStreamUnprepareHeader(hstr, &strhdr, 0);         
	if (mr)
	{
		acmStreamClose(hstr, 0);
		delete[] des_data;
		delete[] pcm_format;       
		delete[] gsmFormat;
	
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}
  
	// close the stream
	acmStreamClose(hstr, 0);

	pszPcmBuf = des_data;
	//if successful returns successful
	return 0;


}//END OF CONVERT FUNCTION




int egConvertGSMBuffTo1Bit( char* source_data,char* des_Data, unsigned int sz)
{

	HMMIO          hmmioIn = NULL;					// handle to open input WAVE file
	HMMIO          hmmioOut = NULL;					// handle to open output WAVE file
	MMCKINFO       ckInRIFF;						// chunk info. for input RIFF chunk
	MMCKINFO       ckOutRIFF;						// chunk info. for output RIFF chunk
	MMCKINFO       ckIn;						    // info. for a chunk in input file
	MMCKINFO       ckOut;					        // info. for a chunk in output file
	MMRESULT	   mr;						        //result of read write operations on file


	


	// Now we locate a CODEC that supports  format tag WAVE_FORMAT_DSPGROUP_TRUESPEECH
	HACMDRIVERID hadid = find_driver(WAVE_FORMAT_DSPGROUP_TRUESPEECH );
	if (hadid == NULL)
	{
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
		
	}



	// get a format The driver supports with specific format tag
	WAVEFORMATEX *true_speech_format = get_driver_format(hadid,
		                                      WAVE_FORMAT_DSPGROUP_TRUESPEECH );
	if (true_speech_format == NULL)
	{
	
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}


	// Now we locate a CODEC that supports  format tag WAVE_FORMAT_DSPGROUP_TRUESPEECH
	HACMDRIVERID hadidGSM = find_driver(WAVE_FORMAT_GSM610);
	if (hadid == NULL)
	{
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
		
	}


    //get the format the driver with PCM format tag  
	WAVEFORMATEX *pcm_format = get_driver_format(hadidGSM,WAVE_FORMAT_GSM610);
	if (pcm_format == NULL)
	{
		delete[] true_speech_format;
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}


	//open the driver for conversion and get its handle in had
	HACMDRIVER had = NULL;
	mr = acmDriverOpen(&had, hadid, 0);
	if (mr)
	{
		delete[] true_speech_format;
		delete[] pcm_format;
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}

	


	//open the stream for conversion for converting from 
	//pcm to true speech format get its handle in hstr
	
	//setbthe des format as per the requirement
	WAVEFORMATEX   *des_format;
    
	HACMSTREAM hstr = NULL;

	mr = acmStreamOpen(&hstr,						   //stream		
							NULL, //had,						   // any driver
							pcm_format, 	               // source format  .....
							true_speech_format,    	       // destination format
							NULL,					       // no filter
							0,				      	       // no callback
							0,						       // instance data (not used)
							ACM_STREAMOPENF_NONREALTIME);  // flags
		if (mr) 
		{
			delete[] pcm_format;       
			delete[] true_speech_format;

			egerrno = _EG_NOWAVEDRIVER;
			return -1;
		}
		//set destination format to be true speech 
	des_format    = true_speech_format;


	
	// allocate a buffer for the result of the conversion
	//depending on the total time of play b'cos total time of paly 
	//for both the file would be same
	DWORD source_tot_bytes_per_sec = pcm_format->nAvgBytesPerSec;
	double tot_time_of_play        = (double)(sz) / source_tot_bytes_per_sec;

	//calculate the des data (final) 
	DWORD des_tot_bytes_per_sec = des_format->nAvgBytesPerSec;
	double real_des_data_size = ((double)(tot_time_of_play * des_tot_bytes_per_sec));

	//get the highest integer value
	DWORD des_data_size = (DWORD) ceil(real_des_data_size);
	
	//assign the mem to final data 
	BYTE* des_data = (BYTE*) malloc(des_data_size + 500);
	
	
	#ifdef _DEBUG
	// fill the dest buffer with zeroes just so we can see if anything got 
	// converted in the debugger
	memset((void*)des_data, 0, (des_data_size));
	#endif

	// fill in the conversion info in ACMSTREAMHEADER structure to be converted
	ACMSTREAMHEADER strhdr;

	memset(&strhdr, 0, sizeof(strhdr));

	strhdr.cbStruct		    = sizeof(strhdr);
	strhdr.fdwStatus		= 0L;
	strhdr.dwUser			= 0L;
	strhdr.pbSrc			= (LPBYTE)source_data; // the source data to convert
	strhdr.cbSrcLength		= sz;
	strhdr.cbSrcLengthUsed  = 0L;
	strhdr.dwSrcUser        = 0L;
	strhdr.pbDst            = (LPBYTE)des_data;
	strhdr.cbDstLength      = des_data_size;
	strhdr.cbDstLengthUsed  = 0L;
	strhdr.dwDstUser        = 0L; 

	// prep the header to convert the data 
	mr = acmStreamPrepareHeader(hstr, &strhdr, 0L); 


	
	// convert the data to the final format
	mr = acmStreamConvert(hstr, &strhdr, 0);
	if (mr)
	{
		acmStreamClose(hstr, 0);
		delete[] des_data;
		delete[] source_data;
		delete[] pcm_format;       
		delete[] true_speech_format;
		
		egerrno = _EG_NOWAVEDRIVER;
		return -1;

	}
	
	//unprepare the stream for future use
	mr =acmStreamUnprepareHeader(hstr, &strhdr, 0);         
	if (mr)
	{
		acmStreamClose(hstr, 0);
		delete[] pcm_format;       
		delete[] true_speech_format;
	
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}
  

	// close the stream
	acmStreamClose(hstr, 0);

	//if successful returns successful
	return 0;


}//END OF CONVERT FUNCTION


/***********************************************************
* [25-10-2016]	
*************************************************************/
int egConvertGSMTo1Bit( char* source_file,char* des_file)
{

	HMMIO          hmmioIn = NULL;					// handle to open input WAVE file
	HMMIO          hmmioOut = NULL;					// handle to open output WAVE file
	MMCKINFO       ckInRIFF;						// chunk info. for input RIFF chunk
	MMCKINFO       ckOutRIFF;						// chunk info. for output RIFF chunk
	MMCKINFO       ckIn;						    // info. for a chunk in input file
	MMCKINFO       ckOut;					        // info. for a chunk in output file
	MMRESULT	   mr;						        //result of read write operations on file


	


	// Open the input file for reading using buffered I/O.
	hmmioIn = mmioOpen(source_file, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmioIn == NULL)
	{
		egerrno = _EG_BADFILE;
		return -1;
	}

	//Descend the input file into the 'RIFF' chunk.
	if (mmioDescend(hmmioIn, &ckInRIFF, NULL, 0) != 0)
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Make sure the input file is a WAVE file.
	if ((ckInRIFF.ckid != FOURCC_RIFF) || 
		 ckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// Search the input file for for the 'fmt ' chunk.
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	
	//assign memory to source_format
	WAVEFORMATEX *source_format = (WAVEFORMATEX*)(new BYTE[sizeof(WAVEFORMATEX)]);
	if( !source_format )
	{
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
	
	// Read the 'fmt ' chunk data into <source_format>.
	if (mmioRead(hmmioIn, (HPSTR)source_format,
		(long) sizeof(*source_format)) != (long) sizeof(*source_format))
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'fmt ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}


	// Search the input file for for the 'data' chunk.
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmioIn, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}


	// reading the data size of input file   
	DWORD   source_data_size = ckIn.cksize;
	if( !source_data_size )
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	//assign the memory to the source_dat
	HPSTR   source_data = (HPSTR)(new BYTE[source_data_size]);
	if( !source_data )
	{
		mmioClose(hmmioIn, 0);
		delete[] source_format;
		egerrno = _EG_MEMERR;
		return -1;
	}

	//read the data of saurce file in the source_data variable 
	//if fail to read then return the func.
	if(mmioRead (hmmioIn, source_data, source_data_size) != LONG(source_data_size))
	{
		mmioClose(hmmioIn, 0);
		delete[] source_data;
		delete[] source_format;
		egerrno = _EG_BADFILE;
		return -1;
	} 
	
	// comes out of the input file out of the 'data ' chunk.
	if (mmioAscend(hmmioIn, &ckIn, 0) != 0)
	{
		delete[] source_format;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}

	// comes out of the input file out of the 'RIFF ' chunk.
	if (mmioAscend(hmmioIn, &ckInRIFF, 0) != 0)
	{
		delete[] source_format;
		delete[] source_data;
		mmioClose(hmmioIn, 0);
		egerrno = _EG_BADFILE;
		return -1;
	}
	
	//close the input file
	mmioClose(hmmioIn, 0);


	// Now we locate a CODEC that supports  format tag WAVE_FORMAT_DSPGROUP_TRUESPEECH
	HACMDRIVERID hadid = find_driver(WAVE_FORMAT_DSPGROUP_TRUESPEECH );
	if (hadid == NULL)
	{
		delete[] source_data;
		delete[] source_format;
		
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
		
	}



	// get a format The driver supports with specific format tag
	WAVEFORMATEX *true_speech_format = get_driver_format(hadid,
		                                      WAVE_FORMAT_DSPGROUP_TRUESPEECH );
	if (true_speech_format == NULL)
	{
		delete[] source_data;
		delete[] source_format;
		
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}


    //get the format the driver with PCM format tag  
	WAVEFORMATEX *pcm_format = get_driver_format(hadid,WAVE_FORMAT_GSM610);
	if (pcm_format == NULL)
	{
		delete[] source_data;
		delete[] source_format;
		delete[] true_speech_format;
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}


	//open the driver for conversion and get its handle in had
	HACMDRIVER had = NULL;
	mr = acmDriverOpen(&had, hadid, 0);
	if (mr)
	{
		delete[] source_data;
		delete[] source_format;
		delete[] true_speech_format;
		delete[] pcm_format;
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}

	


	//open the stream for conversion for converting from 
	//pcm to true speech format get its handle in hstr
	
	//setbthe des format as per the requirement
	WAVEFORMATEX   *des_format;
    
	HACMSTREAM hstr = NULL;

	//if the source format is pcm open the stream to convert the data to TRUE SPEECH
	if(source_format->wFormatTag == WAVE_FORMAT_GSM610)
	{
		mr = acmStreamOpen(&hstr,						   //stream		
							had,						   // any driver
							pcm_format, 	               // source format  .....
							true_speech_format,    	       // destination format
							NULL,					       // no filter
							0,				      	       // no callback
							0,						       // instance data (not used)
							ACM_STREAMOPENF_NONREALTIME);  // flags
		if (mr) 
		{
			delete[] source_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;

			egerrno = _EG_NOWAVEDRIVER;
			return -1;
		}
		//set destination format to be true speech 
		des_format    = true_speech_format;

	}	//end of if the source format is PCM
	else		//when source format is true speech open the stream to 
	{			//convert the data to PCM 
	
		mr = acmStreamOpen(&hstr,						   //stream		
							had,						   // any driver
							true_speech_format,	           // source format  .....
							pcm_format,    				   // destination format
							NULL,					       // no filter
							0,				      	       // no callback
							0,						       // instance data (not used)
							ACM_STREAMOPENF_NONREALTIME);  // flags
		if (mr) 
		{

			delete[] source_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;
        	
			egerrno = _EG_NOWAVEDRIVER;
			return -1;
		}
		//set destination format to be PCM format 
		des_format    =  pcm_format;
	
	}//end of else block if source format is true speech


	
	
	// allocate a buffer for the result of the conversion
	//depending on the total time of play b'cos total time of paly 
	//for both the file would be same
	DWORD source_tot_bytes_per_sec = source_format->nAvgBytesPerSec;
	double tot_time_of_play        = (double)(source_data_size) / source_tot_bytes_per_sec;

	//calculate the des data (final) 
	DWORD des_tot_bytes_per_sec = des_format->nAvgBytesPerSec;
	double real_des_data_size = ((double)(tot_time_of_play * des_tot_bytes_per_sec));

	//get the highest integer value
	DWORD des_data_size = (DWORD) ceil(real_des_data_size);
	
	//assign the mem to final data 
	BYTE* des_data = (BYTE*) malloc(des_data_size + 500);
	
	
	#ifdef _DEBUG
	// fill the dest buffer with zeroes just so we can see if anything got 
	// converted in the debugger
	memset((void*)des_data, 0, (des_data_size));
	#endif

	// fill in the conversion info in ACMSTREAMHEADER structure to be converted
	ACMSTREAMHEADER strhdr;

	memset(&strhdr, 0, sizeof(strhdr));

	strhdr.cbStruct		    = sizeof(strhdr);
	strhdr.fdwStatus		= 0L;
	strhdr.dwUser			= 0L;
	strhdr.pbSrc			= (LPBYTE)source_data; // the source data to convert
	strhdr.cbSrcLength		= source_data_size;
	strhdr.cbSrcLengthUsed  = 0L;
	strhdr.dwSrcUser        = 0L;
	strhdr.pbDst            = (LPBYTE)des_data;
	strhdr.cbDstLength      = des_data_size;
	strhdr.cbDstLengthUsed  = 0L;
	strhdr.dwDstUser        = 0L; 

	// prep the header to convert the data 
	mr = acmStreamPrepareHeader(hstr, &strhdr, 0L); 


	
	// convert the data to the final format
	mr = acmStreamConvert(hstr, &strhdr, 0);
	if (mr)
	{
		acmStreamClose(hstr, 0);
		delete[] des_data;
		delete[] source_data;
	    delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		
		egerrno = _EG_NOWAVEDRIVER;
		return -1;

	}
	
	//unprepare the stream for future use
	mr =acmStreamUnprepareHeader(hstr, &strhdr, 0);         
	if (mr)
	{
		acmStreamClose(hstr, 0);
		delete[] des_data;
		delete[] source_data;
	    delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
	
		egerrno = _EG_NOWAVEDRIVER;
		return -1;
	}
  
	//delete format and data cos this is no more needed
	delete[] source_data; 
	
	
	// close the stream
	acmStreamClose(hstr, 0);

	// Open the output file for writing using buffered I/O. 
	hmmioOut = mmioOpen((char*)des_file,
		                 NULL,
						 MMIO_ALLOCBUF | MMIO_WRITE | MMIO_CREATE|MMIO_EXCLUSIVE);
	if (hmmioOut == NULL)
	{
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;

		egerrno = _EG_WRITEERROR;
		return -1;
	}


	//Create the output file RIFF chunk of form type 'WAVE'.
	ckOutRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if (mmioCreateChunk(hmmioOut, &ckOutRIFF, MMIO_CREATERIFF) != MMSYSERR_NOERROR )
	{
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		mmioClose(hmmioOut, 0);

		egerrno = _EG_WRITEERROR;
		return -1;

	}

	
	//create the memory chunk for format
	long dw = SIZEOF_WAVEFORMATEX(des_format);
	ckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioCreateChunk(hmmioOut, &ckOut, 0) != MMSYSERR_NOERROR)
	{ 
		// cannot write file, probably
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		mmioClose(hmmioOut, 0);
		
		egerrno = _EG_WRITEERROR;
		return -1;
	}

	//write format chunk and ascend
	if (mmioWrite(hmmioOut, (HPSTR)des_format, dw) != (LONG)dw)
	{
		// cannot write file, probably
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		mmioClose(hmmioOut, 0);
		
		egerrno = _EG_WRITEERROR;
		return -1;
	}

	if (mmioAscend(hmmioOut, &ckOut, 0) == MMIOERR_CANNOTWRITE )
	{
		// cannot write file, probably
		delete[] des_data;
		delete[] source_format;
		delete[] pcm_format;       
		delete[] true_speech_format;
		mmioClose(hmmioOut, 0);

		egerrno = _EG_WRITEERROR;
		return -1;

	}

	//if source format is DSP group than this fact chunk is required 
	if(des_format->wFormatTag == WAVE_FORMAT_DSPGROUP_TRUESPEECH)
	{

		//set no of samples to be playe after decompreesion
		DWORD cSamples = (DWORD)(source_format->nSamplesPerSec*(tot_time_of_play)); 
		ckOut.ckid   = mmioFOURCC('f', 'a', 'c', 't');
		ckOut.cksize = cSamples;
		if(mmioCreateChunk(hmmioOut, &ckOut, 0) != MMSYSERR_NOERROR ) 
		{
			// cannot write file, probably
			delete[] des_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;	
			mmioClose(hmmioOut, 0);
		
			egerrno = _EG_WRITEERROR;
			return -1;

		}
			
		
		if(mmioWrite(hmmioOut, (HPSTR)&cSamples, sizeof(cSamples)) !=  sizeof(cSamples))
		{
			// cannot write file, probably
			delete[] des_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;
			mmioClose(hmmioOut, 0);
			
			egerrno = _EG_WRITEERROR;
			return -1;
		}
		if(mmioAscend(hmmioOut, &ckOut, 0) == MMIOERR_CANNOTWRITE )
		{
			// cannot write file, probably
			delete[] des_data;
			delete[] source_format;
			delete[] pcm_format;       
			delete[] true_speech_format;
			mmioClose(hmmioOut, 0);

			egerrno = _EG_WRITEERROR;
			return -1;
		}
	}//end of outer if block

	//no need of these now so delete these
	delete[] source_format;
	delete[] pcm_format;       
	delete[] true_speech_format;
	
	//create the memory chunk for format
	ckOut.ckid = mmioFOURCC('d', 'a', 't', 'a');
	ckOut.cksize = des_data_size;
	if (mmioCreateChunk(hmmioOut, &ckOut,0) != MMSYSERR_NOERROR )
	{
		// cannot write file, probably
		delete[] des_data;
		mmioClose(hmmioOut, 0);

		egerrno = _EG_WRITEERROR;
		return -1;

	}


	// Create the 'data' chunk that holds the waveform samples.and ascends out of it
	if(mmioWrite (hmmioOut, 
		         (LPCTSTR)des_data,
				  des_data_size) != LONG(des_data_size))
	{

		delete[] des_data;
		mmioClose(hmmioOut, 0);
	
		egerrno = _EG_WRITEERROR;
		return -1;

	}
	//delete data buffer
	delete[] des_data;

	//comes out of file riff chunk
	if (mmioAscend(hmmioOut, &ckOut, 0) == MMIOERR_CANNOTWRITE )
	{
		mmioClose(hmmioOut, 0);
		
		egerrno = _EG_WRITEERROR;
		return -1;
	}


	//ascend out of riff chunk
	if (mmioAscend(hmmioOut, &ckOutRIFF, 0) == MMIOERR_CANNOTWRITE )
	{
		mmioClose(hmmioOut, 0);

		egerrno = _EG_WRITEERROR;
		return -1;
	}


	if (hmmioOut != NULL)
		mmioClose(hmmioOut, 0);

	//if successful returns successful
	return 0;


}//END OF CONVERT FUNCTION



