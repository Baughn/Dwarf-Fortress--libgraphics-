#ifndef TEXTLINES_H
#define TEXTLINES_H

struct textlinesst
{
	svector<char *> text;

	void load_raw_to_lines(char *filename);
	void save_lines(char *outputfile);
	char load_lines(char *filename,char initial_only=0);
	char load_lines(string &filename,char initial_only=0);

	~textlinesst()
		{
		clean_lines();
		}
	void clean_lines()
		{
		long t;
		for(t=(long)text.size()-1;t>=0;t--)
			{
			if(text[t]!=NULL)delete[] text[t];
			}
		text.clear();
		}
	void copy_from(textlinesst &ot)
		{
		clean_lines();

		text.resize(ot.text.size());
		int t;
		for(t=0;t<ot.text.size();t++)
			{
			if(ot.text[t]!=NULL)
				{
				text[t]=new char[strlen(ot.text[t])+1];
				memmove(text[t],ot.text[t],sizeof(char)*(strlen(ot.text[t])+1));
				}
			else text[t]=NULL;
			}
		}
};

#endif