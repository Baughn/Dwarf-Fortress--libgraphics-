struct tile_pagest
{
	string token;

	string filename;
	short tile_dim_x;
	short tile_dim_y;
	short page_dim_x;
	short page_dim_y;

	svector<long> texpos;
	svector<long> datapos;
	svector<long> texpos_gs;
	svector<long> datapos_gs;

	char loaded;



	tile_pagest()
		{
		loaded=0;
		}

	void load_graphics();
};

class texture_handlerst
{
	public:
		svector<tile_pagest *> page;

		svector<long> texpos;
		svector<long> datapos;

		void clean();
		void adopt_new_lines(textlinesst &lines);

		~texture_handlerst()
			{
			clean();
			}

		tile_pagest *get_tile_page_by_token(string &tk)
			{
			long t;
			for(t=0;t<page.size();t++)
				{
				if(page[t]->token==tk)return page[t];
				}
			return NULL;
			}
};