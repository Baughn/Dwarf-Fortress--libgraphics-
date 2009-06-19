#include "platform.h"
#include <string.h>
#include <math.h>
#include <iosfwd>
#include <iostream>
#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <zlib.h>

#include "svector.h"
using std::string;

#include "endian.h"

#include "files.h"

#include "textlines.h"

#include "enabler.h"

#include "basics.h"

extern char filecomp_buffer[20000];
extern char filecomp_buffer2[80000];
extern char filecomp_buffer_aux[20000];
extern char filecomp_buffer2_aux[80000];


void textlinesst::load_raw_to_lines(char *filename)
{
	clean_lines();

	//LOAD THE FILE
	std::ifstream fseed(filename);
	if(fseed.is_open())
		{
		string str;

		while(std::getline(fseed,str))
			{
			long end=str.length();

			while(end>0)
				{
				if(isspace(str[end-1]))end--;
				else break;
				}

			str.resize(end);

			if(str.length()>0)
				{
				char *newl=new char[str.length()+1];
				strcpy(newl,str.c_str());

				text.push_back(newl);
				}
			}
		}
	fseed.close();
}

void textlinesst::save_lines(char *outputfile)
{
	if(text.size()==0)return;

	file_compressorst filecomp(filecomp_buffer_aux,20000,filecomp_buffer2_aux,80000);

	string of=outputfile;

	filecomp.open_file(of,0);

	long dummy=text.size();
	filecomp.write_file(dummy);
	long t,ind;
	for(t=0;t<text.size();t++)
		{
		dummy=strlen(text[t]);
		filecomp.write_file(dummy);
		for(ind=0;ind<dummy+1;ind++)filecomp.write_file(text[t][ind]);
		}

	filecomp.flush_in_buffer();

	filecomp.close_file();
}

char textlinesst::load_lines(string &filename,char initial_only)
{
	char str[200];
	strcpy(str,filename.c_str());

	return load_lines(str,initial_only);
}

char textlinesst::load_lines(char *filename,char initial_only)
{
	clean_lines();

	file_compressorst filecomp(filecomp_buffer_aux,20000,filecomp_buffer2_aux,80000);

	string iff=filename;

	char ret=1;

	if(filecomp.open_file(iff,1))
		{
		char initcheck=0;

		char realfile[200];
		strcpy(realfile,"");
		long pos=strlen(filename)-1;
		if(pos>=0)
			{
			while(filename[pos]!='\\'&&filename[pos]!='/')
				{
				pos--;
				if(pos<0)break;
				}
			pos++;
			strcpy(realfile,filename+pos);
			}

		long dummy;
		filecomp.read_file(dummy);
		text.resize(dummy);
		long t,ind;
		for(t=0;t<text.size();t++)
			{
			filecomp.read_file(dummy);
			text[t]=new char[dummy+1];
			for(ind=0;ind<dummy+1;ind++)filecomp.read_file(text[t][ind]);

			//CHECK THAT FIRST LINE MATCHES FILE NAME
			if(t==0)
				{
				string chk=realfile;
				string chk2=text[t];
				simplify_string(chk);
				simplify_string(chk2);
				if(chk!=chk2)ret=0;

				initcheck=1;
				}
			else if(initial_only&&initcheck)
				{
				if(strcmp(text[t],"[INITIAL_LOAD]"))ret=0;

				initcheck=0;
				}
			}

		filecomp.close_file();
		}
	else ret=0;

	if(ret==0)clean_lines();

	return ret;
}