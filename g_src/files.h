//Copyright (c) 2006, Tarn Adams
//All rights reserved.  See game.cpp or license.txt for more information.
#ifndef FILES_H
#define FILES_H

#define FILE_IN_BUFF 1000000
#define FILE_OUT_BUFF 2000000

#include <string>
using std::string;

#include <fstream>
#include "endian.h"
#include "svector.h"

class file_compressorst
{
	public:
		bool compressed;
		std::fstream f;


		char open_file(const string &filename,char existing_only);
		void close_file();
		char write_file(string &str);
		char read_file(string &str);

		char load_posnull_pointer();
		char save_posnull_pointer(void *ptr);

		void write_file_fixedlength(char *var)
			{
			short len=strlen(var);
			write_file(len);
			if(len>0)write_file(var,len*sizeof(char));
			}
		void read_file_fixedlength(char *var)
			{
			short len;
			read_file(len);//DO NOT NEED TO ALLOCATE SPACE
			if(len>0)read_file(var,len*sizeof(char));
			var[len]='\x0';
			}
		char write_file(long var)
			{
			var=byteswap(var);
			return write_file(&var,sizeof(long));
			}
		char read_file(long &var)
			{
			char ret = read_file(&var,sizeof(long));
			var = byteswap(var);
			return ret;
			}
		char write_file(int var)
			{
			var=byteswap(var);
			return write_file(&var,sizeof(int));
			}
		char read_file(int &var)
			{
			char ret = read_file(&var,sizeof(int));
			var = byteswap(var);
			return ret;
			}
		char write_file(short var)
			{
			var=byteswap(var);
			return write_file(&var,sizeof(short));
			}
		char read_file(short &var)
			{
			char ret = read_file(&var,sizeof(short));
			var = byteswap(var);
			return ret;
			}
		char write_file(char var)
			{
			return write_file(&var,sizeof(char));
			}
		char read_file(char &var)
			{
			return read_file(&var,sizeof(char));
			}
		char write_file(bool var)
			{
			char temp;
			if(var)temp=1;
			else temp=0;
			return write_file(&temp,sizeof(char));
			}
		char read_file(bool &var)
			{
			char temp;
			if(!read_file(&temp,sizeof(char)))return 0;
			var=(temp!=0);
			return 1;
			}
		void write_file(svector<bool> &vect)
			{
			long s=vect.size();
			write_file(s);
			bool bl;//JUST FOR PARITY WITH read BELOW
			long i;
			for(i=0;i<s;i++)
				{
				bl=vect[i];
				write_file(bl);
				}
			}
		void read_file(svector<bool> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			bool bl;//NO IDEA WHY IT CAN'T JUST TAKE vect[i]
			long i;
			for(i=0;i<s;i++)
				{
				read_file(bl);
				vect[i]=bl;
				}
			}
		char write_file(unsigned long var)
			{
			var=byteswap(var);
			return write_file(&var,sizeof(unsigned long));
			}
		char read_file(unsigned long &var)
			{
			char ret = read_file(&var,sizeof(unsigned long));
			var = byteswap(var);
			return ret;
			}
		char write_file(unsigned int var)
			{
			var=byteswap(var);
			return write_file(&var,sizeof(unsigned int));
			}
		char read_file(unsigned int &var)
			{
			char ret = read_file(&var,sizeof(unsigned int));
			var = byteswap(var);
			return ret;
			}
		char write_file(unsigned short var)
			{
			var=byteswap(var);
			return write_file(&var,sizeof(unsigned short));
			}
		char read_file(unsigned short &var)
			{
			char ret = read_file(&var,sizeof(unsigned short));
			var = byteswap(var);
			return ret;
			}
		char write_file(unsigned char var)
			{
			return write_file(&var,sizeof(unsigned char));
			}
		char read_file(unsigned char &var)
			{
			return read_file(&var,sizeof(unsigned char));
			}
		void write_file(svector<short> &vect)
			{
			long s=vect.size();
			write_file(s);
			long i;
			for(i=0;i<s;i++)
				{
				write_file(vect[i]);
				}
			}
		void read_file(svector<short> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			long i;
			for(i=0;i<s;i++)
				{
				read_file(vect[i]);
				}
			}
		void write_file(svector<unsigned short> &vect)
			{
			long s=vect.size();
			write_file(s);
			long i;
			for(i=0;i<s;i++)
				{
				write_file(vect[i]);
				}
			}
		void read_file(svector<unsigned short> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			long i;
			for(i=0;i<s;i++)
				{
				read_file(vect[i]);
				}
			}
		void write_file(svector<unsigned int> &vect)
			{
			long s=vect.size();
			write_file(s);
			long i;
			for(i=0;i<s;i++)
				{
				write_file(vect[i]);
				}
			}
		void read_file(svector<unsigned int> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			long i;
			for(i=0;i<s;i++)
				{
				read_file(vect[i]);
				}
			}
		void write_file(svector<int> &vect)
			{
			long s=vect.size();
			write_file(s);
			long i;
			for(i=0;i<s;i++)
				{
				write_file(vect[i]);
				}
			}
		void read_file(svector<int> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			long i;
			for(i=0;i<s;i++)
				{
				read_file(vect[i]);
				}
			}
		void write_file(svector<unsigned long> &vect)
			{
			long s=vect.size();
			write_file(s);
			long i;
			for(i=0;i<s;i++)
				{
				write_file(vect[i]);
				}
			}
		void read_file(svector<unsigned long> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			long i;
			for(i=0;i<s;i++)
				{
				read_file(vect[i]);
				}
			}
		void write_file(svector<unsigned char> &vect)
			{
			long s=vect.size();
			write_file(s);
			long i;
			for(i=0;i<s;i++)
				{
				write_file(vect[i]);
				}
			}
		void read_file(svector<unsigned char> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			long i;
			for(i=0;i<s;i++)
				{
				read_file(vect[i]);
				}
			}
		void write_file(svector<char> &vect)
			{
			long s=vect.size();
			write_file(s);
			long i;
			for(i=0;i<s;i++)
				{
				write_file(vect[i]);
				}
			}
		void read_file(svector<char> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			long i;
			for(i=0;i<s;i++)
				{
				read_file(vect[i]);
				}
			}
		void write_file(svector<long> &vect)
			{
			long s=vect.size();
			write_file(s);
			long i;
			for(i=0;i<s;i++)
				{
				write_file(vect[i]);
				}
			}
		void read_file(svector<long> &vect)
			{
			long s;
			read_file(s);
			vect.resize(s);
			long i;
			for(i=0;i<s;i++)
				{
				read_file(vect[i]);
				}
			}

		char load_new_in_buffer();
		char flush_in_buffer();

		file_compressorst();
		file_compressorst(char *new_in_buffer,long new_in_buffersize,
						char *new_out_buffer,long new_out_buffersize);
		~file_compressorst()
			{
			close_file();
			}

	private:
		char write_file(void *write_var,long write_size);
		char read_file(void *read_var,long read_size);

		char *in_buffer;
		long in_buffersize;
		long in_buffer_amount_loaded;
		long in_buffer_position;

		char *out_buffer;
		long out_buffersize;
		long out_buffer_amount_written;

		static char def_ibuff[FILE_IN_BUFF];
		static char def_obuff[FILE_OUT_BUFF];
};

void copy_file(const string &src,const string &dst);
// Replaces dst with src, removing src in the process. Atomic if possible.
void replace_file(const string &src, const string &dst);
#endif
