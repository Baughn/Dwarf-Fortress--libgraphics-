template <class T> int32_t get_vector_sum(svector<T> vec)
{
	T total=0;
	int32_t l;
	for(l=(int32_t)vec.size()-1;l>=0;l--)total+=vec[l];
	return total;
}

template <class T> int32_t get_random_biased_index(svector<T> chance)
{
	int32_t sz=chance.size();
	if(sz==0)
		{
		errorlog_string("Empty biased index vector");
		return -1;
		}

	T roll=trandom(get_vector_sum(chance));

	T cur_chance;
	int32_t l;
	for(l=sz-1;l>=0;l--)
		{
		cur_chance=chance[l];
		if(roll<cur_chance)return l;
		roll-=cur_chance;
		}

	errorlog_string("Biased index vector computation error");
	return 0;//MORE FUNCTIONS WILL BE HAPPIER WITH 0 THAN -1 HERE
}

template <class T> void zero_vector(svector<T> &vc)
{
	int32_t sz=vc.size();
	if(sz==0)return;
	//NOTE: vector MEMORY IS GUARANTEED TO BE CONTIGUOUS, AND THIS IS FASTER THAN GOING THROUGH ONE BY ONE
			//apparently this gives linux a headache though, so back to the slower way
	//memset(&(vc[0]),0,sizeof(T)*sz);
	int32_t i;
	for(i=(int32_t)vc.size()-1;i>=0;--i)vc[i]=0;
}

template <class T> bool positive_vector(svector<T> &vc)
{
	int32_t i;
	for(i=(int32_t)vc.size()-1;i>=0;--i)
		{
		if(vc[i]>0)return true;
		}
	return false;
}

template <class T> void binary_add_unique_to_vector(T nl,svector<T> &vc)
{
	int32_t size=vc.size();
	if(size==0)
		{
		vc.push_back(nl);
		return;
		}
	if(vc[size-1]<nl)
		{
		vc.push_back(nl);
		return;
		}

	binary_add_unique_to_vector_step(0,size-1,nl,vc);
}

template <class T> void binary_add_unique_to_vector_step(int32_t start,int32_t end,T nl,svector<T> &vc)
{
	if(start>end)
		{
		if(end<0)
			{
			if(vc[start]<nl)
				{
				if(start+1>=(int32_t)vc.size())vc.push_back(nl);
				else vc.insert(start+1,nl);
				}
			else vc.insert(start,nl);
			}
		else if(end<(int32_t)vc.size())
			{
			if(vc[end]<nl)
				{
				if(end+1>=(int32_t)vc.size())vc.push_back(nl);
				else vc.insert(end+1,nl);
				}
			else vc.insert(end,nl);
			}
		else vc.push_back(nl);
		return;
		}

	int32_t mid=(start+end)>>1;

	int32_t cptr=vc[mid];
	if(cptr==nl)return;
	if(start==end)
		{
		if(cptr<nl)
			{
			if(start+1>=(int32_t)vc.size())vc.push_back(nl);
			else vc.insert(start+1,nl);
			}
		else vc.insert(start,nl);
		return;
		}

	if(cptr>nl)binary_add_unique_to_vector_step(start,mid-1,nl,vc);
	else binary_add_unique_to_vector_step(mid+1,end,nl,vc);
}

template <class T> void add_unique_to_vector(T nl,svector<T> &vc)
{
	int32_t i;
	for(i=(int32_t)vc.size()-1;i>=0;i--)
		{
		if(vc[i]==nl)return;
		}
	vc.push_back(nl);
}

template <class T,class T2> void add_dual_unique_to_vectors(T nl,T2 nl2,svector<T> &vc,svector<T2> &vc2)
{
	int32_t i;
	for(i=(int32_t)vc.size()-1;i>=0;i--)
		{
		if(vc[i]==nl&&
			vc2[i]==nl2)return;
		}
	vc.push_back(nl);
	vc2.push_back(nl2);
}

template <class T,class T2,class T3,class T4> void add_quad_unique_to_vectors(T nl,T2 nl2,T3 nl3,T4 nl4,
																				svector<T> &vc,svector<T2> &vc2,svector<T3> &vc3,svector<T4> &vc4)
{
	int32_t i;
	for(i=(int32_t)vc.size()-1;i>=0;i--)
		{
		if(vc[i]==nl&&
			vc2[i]==nl2&&
			vc3[i]==nl3&&
			vc4[i]==nl4)return;
		}
	vc.push_back(nl);
	vc2.push_back(nl2);
	vc3.push_back(nl3);
	vc4.push_back(nl4);
}

template <class T> void remove_all_from_vector(T nl,svector<T> &vc)
{
	int32_t i;
	for(i=(int32_t)vc.size()-1;i>=0;i--)
		{
		if(vc[i]==nl)vc.erase(i);
		}
}

template <class T,class T2> void remove_all_from_dual_vectors(T nl,T2 nl2,svector<T> &vc,svector<T2> &vc2)
{
	int32_t i;
	for(i=(int32_t)vc.size()-1;i>=0;i--)
		{
		if(vc[i]==nl&&
			vc2[i]==nl2)
			{
			vc.erase(i);
			vc2.erase(i);
			}
		}
}

template <class T> int32_t get_vector_index(T a,svector<T> &v)
{
	int32_t i;
	for(i=(int32_t)v.size()-1;i>=0;i--)
		{
		if(v[i]==a)return i;
		}
	return -1;
}

template <class T,class T2> int32_t get_dual_vector_index(T a1,T2 a2,svector<T> &v1,svector<T2> &v2)
{
	int32_t i;
	for(i=(int32_t)v1.size()-1;i>=0;i--)
		{
		if(v1[i]==a1&&
			v2[i]==a2)return i;
		}
	return -1;
}

template <class T,class T2,class T3,class T4> int32_t get_quad_vector_index(T a1,T2 a2,T3 a3,T4 a4,
																		svector<T> &v1,svector<T2> &v2,
																		svector<T3> &v3,svector<T4> &v4)
{
	int32_t i;
	for(i=(int32_t)v1.size()-1;i>=0;i--)
		{
		if(v1[i]==a1&&
			v2[i]==a2&&
			v3[i]==a3&&
			v4[i]==a4)return i;
		}
	return -1;
}

template <class T> void merge_vectors(T &master, T &merger)
{
	int32_t master_start=master.size()-1;
	int32_t i,i2;
	for(i=(int32_t)merger.size()-1;i>=0;i--)
		{
		for(i2=master_start;i2>=0;i2--)
			{
			if(merger[i]==master[i2])break;
			}
		if(i2==-1)
			{
			master.push_back(merger[i]);
			}
		}
}

template <class T> int32_t get_common_element_vector_index(T &master, T &merger)
{
	int32_t i,i2;
	for(i=(int32_t)merger.size()-1;i>=0;i--)
		{
		for(i2=(int32_t)master.size()-1;i2>=0;i2--)
			{
			if(merger[i]==master[i2])return i2;
			}
		}

	return -1;
}

template <class T,class T2> void merge_dual_vectors(T &master, T2 &master2, T &merger, T2 &merger2)
{
	int32_t i,i2;
	for(i=(int32_t)merger.size()-1;i>=0;i--)
		{
		for(i2=(int32_t)master.size()-1;i2>=0;i2--)
			{
			if(merger[i]==master[i2]&&
				merger2[i]==master2[i2])break;
			}
		if(i2==-1)
			{
			master.push_back(merger[i]);
			master2.push_back(merger2[i]);
			}
		}
}

template <class T,class T2,class T3,class T4> void merge_quad_vectors(T &master, T2 &master2, T3 &master3, T4 &master4,
																		T &merger, T2 &merger2, T3 &merger3, T4 &merger4)
{
	int32_t i,i2;
	for(i=(int32_t)merger.size()-1;i>=0;i--)
		{
		for(i2=(int32_t)master.size()-1;i2>=0;i2--)
			{
			if(merger[i]==master[i2]&&
				merger2[i]==master2[i2]&&
				merger3[i]==master3[i2]&&
				merger4[i]==master4[i2])break;
			}
		if(i2==-1)
			{
			master.push_back(merger[i]);
			master2.push_back(merger2[i]);
			master3.push_back(merger3[i]);
			master4.push_back(merger4[i]);
			}
		}
}

template <class T> void cull_vectors(T &master,T &cull)
{
	int32_t i,i2;
	for(i=(int32_t)cull.size()-1;i>=0;i--)
		{
		for(i2=(int32_t)master.size()-1;i2>=0;i2--)
			{
			if(cull[i]==master[i2])
				{
				master.erase(i2);
				break;
				}
			}
		}
}

template <class T> void push_on_vector(T &master,T &new_stuff)
{
	int32_t i;
	for(i=0;i<new_stuff.size();i++)master.push_back(new_stuff[i]);
}

template<class T> void add_to_global_id_vector(T ptr,svector<T> &vect)
{
	int32_t size=vect.size();
	if(size==0)
		{
		vect.push_back(ptr);
		return;
		}
	if(vect[size-1]->global_id<ptr->global_id)
		{
		vect.push_back(ptr);
		return;
		}

	add_to_global_id_vector_binary(0,size-1,ptr,vect);
}

template<class T> void add_to_global_id_vector_binary(int32_t start,int32_t end,T ptr,svector<T> &vect)
{
	if(start>end)
		{
		if(end<0)
			{
			T cptr=vect[start];
			if(cptr->global_id<ptr->global_id)
				{
				if(start+1>=(int32_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
				else vect.insert(start+1,ptr);
				}
			else if(cptr->global_id>ptr->global_id)vect.insert(start,ptr);
			}
		else if(end<(int32_t)vect.size())
			{
			T cptr=vect[end];
			if(cptr->global_id<ptr->global_id)
				{
				if(end+1>=(int32_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
				else vect.insert(end+1,ptr);
				}
			else if(cptr->global_id>ptr->global_id)vect.insert(end,ptr);
			}
		return;//push_back() FOR UNIQUE ALREADY HANDLED SO NO else
		}

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->global_id==ptr->global_id)return;
	if(start==end)
		{
		if(cptr->global_id<ptr->global_id)
			{
			if(start+1>=(int32_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
			else vect.insert(start+1,ptr);
			}
		else if(cptr->global_id>ptr->global_id)vect.insert(start,ptr);
		return;
		}

	if(cptr->global_id>ptr->global_id)add_to_global_id_vector_binary(start,mid-1,ptr,vect);
	else add_to_global_id_vector_binary(mid+1,end,ptr,vect);
}

template<class T> void remove_from_global_id_vector(T ptr,svector<T> &vect)
{
	remove_from_global_id_vector_binary(0,(int32_t)vect.size()-1,ptr,vect);
}

template<class T> void remove_from_global_id_vector_binary(int32_t start,int32_t end,T ptr,svector<T> &vect)
{
	if(start>end)return;

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)
		{
		vect.erase(mid);
		return;
		}

	if(cptr->global_id>ptr->global_id)remove_from_global_id_vector_binary(start,mid-1,ptr,vect);
	else remove_from_global_id_vector_binary(mid+1,end,ptr,vect);
}

template<class T> T get_from_global_id_vector(int32_t id,svector<T> &vect)
{
	return get_from_global_id_vector_binary(0,(int32_t)vect.size()-1,id,vect);
}

template<class T> T get_from_global_id_vector_binary(int32_t start,int32_t end,int32_t id,svector<T> &vect)
{
	if(start>end)return NULL;

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->global_id==id)
		{
		return cptr;
		}

	if(cptr->global_id>id)return get_from_global_id_vector_binary(start,mid-1,id,vect);
	else return get_from_global_id_vector_binary(mid+1,end,id,vect);
}

template<class T> int32_t get_index_from_global_id_vector(int32_t id,svector<T> &vect)
{
	return get_index_from_global_id_vector_binary(0,(int32_t)vect.size()-1,id,vect);
}

template<class T> int32_t get_index_from_global_id_vector_binary(int32_t start,int32_t end,int32_t id,svector<T> &vect)
{
	if(start>end)return -1;

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->global_id==id)
		{
		return mid;
		}

	if(cptr->global_id>id)return get_index_from_global_id_vector_binary(start,mid-1,id,vect);
	else return get_index_from_global_id_vector_binary(mid+1,end,id,vect);
}

template<class T> VIndex add_to_binary_vector(T ptr,svector<T> &vect)
{
	int32_t size=vect.size();
	if(size==0)
		{
		vect.push_back(ptr);
		return 0;
		}
	if(vect[size-1]<ptr)
		{
		vect.push_back(ptr);
		return size;
		}

	return add_to_binary_vector_binary(0,size-1,ptr,vect);
}

template<class T> VIndex add_to_binary_vector_binary(int32_t start,int32_t end,T ptr,svector<T> &vect)
{
	if(start>end)
		{
		if(end<0)
			{
			T cptr=vect[start];
			if(cptr<ptr)
				{
				if(start+1>=(int32_t)vect.size())vect.push_back(ptr);
				else vect.insert(start+1,ptr);
				return start+1;
				}
			else
				{
				vect.insert(start,ptr);
				return start;
				}
			}
		else if(end<(int32_t)vect.size())
			{
			T cptr=vect[end];
			if(cptr<ptr)
				{
				if(end+1>=(int32_t)vect.size())vect.push_back(ptr);
				else vect.insert(end+1,ptr);
				return end+1;
				}
			else
				{
				vect.insert(end,ptr);
				return end;
				}
			}
		else vect.push_back(ptr);
		return end;
		}

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)vect.insert(mid,ptr);//INSERT A COPY HERE SINCE THIS IS A NON-UNIQUE VECTOR
	if(start==end)
		{
		if(cptr<ptr)
			{
			if(start+1>=(int32_t)vect.size())vect.push_back(ptr);
			else vect.insert(start+1,ptr);
			return start+1;
			}
		else
			{
			vect.insert(start,ptr);
			return start;
			}
		}

	if(cptr>ptr)return add_to_binary_vector_binary(start,mid-1,ptr,vect);
	else return add_to_binary_vector_binary(mid+1,end,ptr,vect);
}

//NOTE: RETURNS -1 IF ALREADY PRESENT, NOT THE INDEX
template<class T> VIndex add_unique_to_binary_vector(T ptr,svector<T> &vect)
{
	int32_t size=vect.size();
	if(size==0)
		{
		vect.push_back(ptr);
		return 0;
		}
	if(vect[size-1]<ptr)
		{
		vect.push_back(ptr);
		return size;
		}

	return add_unique_to_binary_vector_binary(0,size-1,ptr,vect);
}

//NOTE: RETURNS -1 IF ALREADY PRESENT, NOT THE INDEX
template<class T> VIndex add_unique_to_binary_vector_binary(int32_t start,int32_t end,T ptr,svector<T> &vect)
{
	if(start>end)
		{
		if(end<0)
			{
			T cptr=vect[start];
			if(cptr<ptr)
				{
				if(start+1>=(int32_t)vect.size())return -1;//push_back() CASE ALREADY HANDLED

				vect.insert(start+1,ptr);
				return start+1;
				}
			else if(cptr>ptr)
				{
				vect.insert(start,ptr);
				return start;
				}
			}
		else if(end<(int32_t)vect.size())
			{
			T cptr=vect[end];
			if(cptr<ptr)
				{
				if(end+1>=(int32_t)vect.size())return -1;//push_back() CASE ALREADY HANDLED

				vect.insert(end+1,ptr);
				return end+1;
				}
			else if(cptr>ptr)
				{
				vect.insert(end,ptr);
				return end;
				}
			}
		return -1;//push_back() CASE ALREADY HANDLED SO NO else
		}

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)return -1;//ALREADY IN VECTOR
	if(start==end)
		{
		if(cptr<ptr)
			{
			if(start+1>=(int32_t)vect.size())return -1;//push_back() CASE ALREADY HANDLED

			vect.insert(start+1,ptr);
			return start+1;
			}
		else if(cptr>ptr)
			{
			vect.insert(start,ptr);
			return start;
			}
		return -1;
		}

	if(cptr>ptr)return add_unique_to_binary_vector_binary(start,mid-1,ptr,vect);
	else return add_unique_to_binary_vector_binary(mid+1,end,ptr,vect);
}

template<class T> void remove_from_binary_vector(T ptr,svector<T> &vect)
{
	remove_from_binary_vector_binary(0,(int32_t)vect.size()-1,ptr,vect);
}

template<class T> void remove_from_binary_vector_binary(int32_t start,int32_t end,T ptr,svector<T> &vect)
{
	if(start>end)return;

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)
		{
		vect.erase(mid);
		return;
		}

	if(cptr>ptr)remove_from_binary_vector_binary(start,mid-1,ptr,vect);
	else remove_from_binary_vector_binary(mid+1,end,ptr,vect);
}

template<class T> int32_t get_index_from_binary_vector(T id,svector<T> &vect)
{
	if(vect.size()==0||id==-1)return -1;

	return get_index_from_binary_vector_binary(0,(int32_t)vect.size()-1,id,vect);
}

template<class T> int32_t get_index_from_binary_vector_binary(int32_t start,int32_t end,T id,svector<T> &vect)
{
	if(start>end)return -1;

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==id)
		{
		return mid;
		}

	if(cptr>id)return get_index_from_binary_vector_binary(start,mid-1,id,vect);
	else return get_index_from_binary_vector_binary(mid+1,end,id,vect);
}

template<class T> int32_t get_floor_index_from_binary_vector(T ptr,svector<T> &vect)
{
	int32_t size=vect.size();
	if(size==0)return -1;
	if(vect[size-1]<ptr)return size-1;

	return get_floor_index_from_binary_vector_binary(0,size-1,ptr,vect);
}

template<class T> int32_t get_floor_index_from_binary_vector_binary(int32_t start,int32_t end,T ptr,svector<T> &vect)
{
	if(start>end)
		{
		if(end<0)
			{
			T cptr=vect[start];
			if(cptr<ptr)
				{
				if(start+1>=(int32_t)vect.size())return start;
				else return start;
				}
			else if(cptr>ptr)
				{
				if(start<=0)return 0;
				else return start-1;
				}
			}
		else if(end<(int32_t)vect.size())
			{
			T cptr=vect[end];
			if(cptr<ptr)
				{
				if(end+1>=(int32_t)vect.size())return end;
				else return end;
				}
			else if(cptr>ptr)
				{
				if(end<=0)return 0;
				else return end-1;
				}
			}
		return start;
		}

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)
		{
		while(mid>0)
			{
			if(vect[mid-1]<ptr)return mid;
			mid--;
			}
		return mid;
		}
	if(start==end)
		{
		if(cptr<ptr)
			{
			if(start+1>=(int32_t)vect.size())return start;
			else return start;
			}
		//NOTE: cptr>ptr as == is already handled above
		else
			{
			if(start<=0)return 0;
			else return start-1;
			}
		}

	if(cptr>ptr)return get_floor_index_from_binary_vector_binary(start,mid-1,ptr,vect);
	else return get_floor_index_from_binary_vector_binary(mid+1,end,ptr,vect);
}



template<class T> void fixed_array_push_back(T ptr,T *vect,int32_t &size,int32_t max)
{
	if(size>=max)return;
	vect[size]=ptr;
	size++;
}

template<class T> void fixed_array_insert(int32_t index,T ptr,T *vect,int32_t &size,int32_t max)
{
	if(size>=max)return;
	int32_t l;
	for(l=size-1;l>=index;l--)vect[l+1]=vect[l];
	vect[index]=ptr;
	size++;
}

template<class T> void add_to_fixed_binary_array(T ptr,T *vect,int32_t &size,int32_t max)
{
	if(size==0)
		{
		fixed_array_push_back(ptr,vect,size,max);
		return;
		}
	if(vect[size-1]<ptr)
		{
		fixed_array_push_back(ptr,vect,size,max);
		return;
		}

	add_to_fixed_binary_array_binary(0,size-1,ptr,vect,size,max);
}

template<class T> void add_to_fixed_binary_array_binary(int32_t start,int32_t end,T ptr,T *vect,int32_t &size,int32_t max)
{
	if(start>end)
		{
		if(end<0)
			{
			T cptr=vect[start];
			if(cptr<ptr)
				{
				if(start+1>=size)fixed_array_push_back(ptr,vect,size,max);
				else fixed_array_insert(start+1,ptr,vect,size,max);
				}
			else fixed_array_insert(start,ptr,vect,size,max);
			}
		else if(end<size)
			{
			T cptr=vect[end];
			if(cptr<ptr)
				{
				if(end+1>=size)fixed_array_push_back(ptr,vect,size,max);
				else fixed_array_insert(end+1,ptr,vect,size,max);
				}
			else fixed_array_insert(end,ptr,vect,size,max);
			}
		else fixed_array_push_back(ptr,vect,size,max);
		return;
		}

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)fixed_array_insert(mid,ptr,vect,size,max);//INSERT A COPY SINCE THIS IS NON-UNIQUE CASE
	if(start==end)
		{
		if(cptr<ptr)
			{
			if(start+1>=size)fixed_array_push_back(ptr,vect,size,max);
			else fixed_array_insert(start+1,ptr,vect,size,max);
			}
		else fixed_array_insert(start,ptr,vect,size,max);
		return;
		}

	if(cptr>ptr)add_to_fixed_binary_array_binary(start,mid-1,ptr,vect,size,max);
	else add_to_fixed_binary_array_binary(mid+1,end,ptr,vect,size,max);
}

template<class T> void add_unique_to_fixed_binary_array(T ptr,T *vect,int32_t &size,int32_t max)
{
	if(size==0)
		{
		fixed_array_push_back(ptr,vect,size,max);
		return;
		}
	if(vect[size-1]<ptr)
		{
		fixed_array_push_back(ptr,vect,size,max);
		return;
		}

	add_unique_to_fixed_binary_array_binary(0,size-1,ptr,vect,size,max);
}

template<class T> void add_unique_to_fixed_binary_array_binary(int32_t start,int32_t end,T ptr,T *vect,int32_t &size,int32_t max)
{
	if(start>end)
		{
		if(end<0)
			{
			T cptr=vect[start];
			if(cptr<ptr)
				{
				if(start+1>=size)return;//WAS push_back()
				else fixed_array_insert(start+1,ptr,vect,size,max);
				}
			else if(cptr>ptr)fixed_array_insert(start,ptr,vect,size,max);
			}
		else if(end<size)
			{
			T cptr=vect[end];
			if(cptr<ptr)
				{
				if(end+1>=size)return;//WAS push_back()
				else fixed_array_insert(end+1,ptr,vect,size,max);
				}
			else if(cptr>ptr)fixed_array_insert(end,ptr,vect,size,max);
			}
		//NOTE: NO else CASE HERE BECAUSE IN THE NON-UNIQUE VERSION IT JUST PUSHED BACK
			//AND PUSHING BACK IS HANDLED IN THE OPENING FUNCTION

		return;
		}

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)return;
	if(start==end)
		{
		if(cptr<ptr)
			{
			if(start+1>=size)return;//WAS push_back()
			else fixed_array_insert(start+1,ptr,vect,size,max);
			}
		else if(cptr>ptr)fixed_array_insert(start,ptr,vect,size,max);
		return;
		}

	if(cptr>ptr)add_unique_to_fixed_binary_array_binary(start,mid-1,ptr,vect,size,max);
	else add_unique_to_fixed_binary_array_binary(mid+1,end,ptr,vect,size,max);
}

template<class T> void add_to_local_id_vector(T ptr,svector<T> &vect)
{
	int32_t size=vect.size();
	if(size==0)
		{
		vect.push_back(ptr);
		return;
		}
	if(vect[size-1]->local_id<ptr->local_id)
		{
		vect.push_back(ptr);
		return;
		}

	add_to_local_id_vector_binary(0,size-1,ptr,vect);
}

template<class T> void add_to_local_id_vector_binary(int32_t start,int32_t end,T ptr,svector<T> &vect)
{
	if(start>end)
		{
		if(end<0)
			{
			T cptr=vect[start];
			if(cptr->local_id<ptr->local_id)
				{
				if(start+1>=(int32_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
				else vect.insert(start+1,ptr);
				}
			else if(cptr->local_id>ptr->local_id)vect.insert(start,ptr);
			}
		else if(end<(int32_t)vect.size())
			{
			T cptr=vect[end];
			if(cptr->local_id<ptr->local_id)
				{
				if(end+1>=(int32_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
				else vect.insert(end+1,ptr);
				}
			else if(cptr->local_id>ptr->local_id)vect.insert(end,ptr);
			}
		return;//push_back() FOR UNIQUE ALREADY HANDLED SO NO else
		}

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->local_id==ptr->local_id)return;
	if(start==end)
		{
		if(cptr->local_id<ptr->local_id)
			{
			if(start+1>=(int32_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
			else vect.insert(start+1,ptr);
			}
		else if(cptr->local_id>ptr->local_id)vect.insert(start,ptr);
		return;
		}

	if(cptr->local_id>ptr->local_id)add_to_local_id_vector_binary(start,mid-1,ptr,vect);
	else add_to_local_id_vector_binary(mid+1,end,ptr,vect);
}

template<class T> void remove_from_local_id_vector(T ptr,svector<T> &vect)
{
	remove_from_local_id_vector_binary(0,(int32_t)vect.size()-1,ptr,vect);
}

template<class T> void remove_from_local_id_vector_binary(int32_t start,int32_t end,T ptr,svector<T> &vect)
{
	if(start>end)return;

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)
		{
		vect.erase(mid);
		return;
		}

	if(cptr->local_id>ptr->local_id)remove_from_local_id_vector_binary(start,mid-1,ptr,vect);
	else remove_from_local_id_vector_binary(mid+1,end,ptr,vect);
}

template<class T> T get_from_local_id_vector(int32_t id,svector<T> &vect)
{
	return get_from_local_id_vector_binary(0,(int32_t)vect.size()-1,id,vect);
}

template<class T> T get_from_local_id_vector_binary(int32_t start,int32_t end,int32_t id,svector<T> &vect)
{
	if(start>end)return NULL;

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->local_id==id)
		{
		return cptr;
		}

	if(cptr->local_id>id)return get_from_local_id_vector_binary(start,mid-1,id,vect);
	else return get_from_local_id_vector_binary(mid+1,end,id,vect);
}

template<class T> int32_t get_index_from_local_id_vector(int32_t id,svector<T> &vect)
{
	return get_index_from_local_id_vector_binary(0,(int32_t)vect.size()-1,id,vect);
}

template<class T> int32_t get_index_from_local_id_vector_binary(int32_t start,int32_t end,int32_t id,svector<T> &vect)
{
	if(start>end)return -1;

	int32_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->local_id==id)
		{
		return mid;
		}

	if(cptr->local_id>id)return get_index_from_local_id_vector_binary(start,mid-1,id,vect);
	else return get_index_from_local_id_vector_binary(mid+1,end,id,vect);
}

template<class T> void add_to_short_id_vector(T ptr,svector<T> &vect)
{
	int16_t size=vect.size();
	if(size==0)
		{
		vect.push_back(ptr);
		return;
		}
	if(vect[size-1]->short_id<ptr->short_id)
		{
		vect.push_back(ptr);
		return;
		}

	add_to_short_id_vector_binary(0,size-1,ptr,vect);
}

template<class T> void add_to_short_id_vector_binary(int16_t start,int16_t end,T ptr,svector<T> &vect)
{
	if(start>end)
		{
		if(end<0)
			{
			T cptr=vect[start];
			if(cptr->short_id<ptr->short_id)
				{
				if(start+1>=(int16_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
				else vect.insert(start+1,ptr);
				}
			else if(cptr->short_id>ptr->short_id)vect.insert(start,ptr);
			}
		else if(end<(int16_t)vect.size())
			{
			T cptr=vect[end];
			if(cptr->short_id<ptr->short_id)
				{
				if(end+1>=(int16_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
				else vect.insert(end+1,ptr);
				}
			else if(cptr->short_id>ptr->short_id)vect.insert(end,ptr);
			}
		return;//push_back() FOR UNIQUE ALREADY HANDLED SO NO else
		}

	int16_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->short_id==ptr->short_id)return;
	if(start==end)
		{
		if(cptr->short_id<ptr->short_id)
			{
			if(start+1>=(int16_t)vect.size())return;//push_back() FOR UNIQUE ALREADY HANDLED
			else vect.insert(start+1,ptr);
			}
		else if(cptr->short_id>ptr->short_id)vect.insert(start,ptr);
		return;
		}

	if(cptr->short_id>ptr->short_id)add_to_short_id_vector_binary(start,mid-1,ptr,vect);
	else add_to_short_id_vector_binary(mid+1,end,ptr,vect);
}

template<class T> void remove_from_short_id_vector(T ptr,svector<T> &vect)
{
	remove_from_short_id_vector_binary(0,(int16_t)vect.size()-1,ptr,vect);
}

template<class T> void remove_from_short_id_vector_binary(int16_t start,int16_t end,T ptr,svector<T> &vect)
{
	if(start>end)return;

	int16_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr==ptr)
		{
		vect.erase(mid);
		return;
		}

	if(cptr->short_id>ptr->short_id)remove_from_short_id_vector_binary(start,mid-1,ptr,vect);
	else remove_from_short_id_vector_binary(mid+1,end,ptr,vect);
}

template<class T> T get_from_short_id_vector(int16_t id,svector<T> &vect)
{
	return get_from_short_id_vector_binary(0,(int16_t)vect.size()-1,id,vect);
}

template<class T> T get_from_short_id_vector_binary(int16_t start,int16_t end,int16_t id,svector<T> &vect)
{
	if(start>end)return NULL;

	int16_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->short_id==id)
		{
		return cptr;
		}

	if(cptr->short_id>id)return get_from_short_id_vector_binary(start,mid-1,id,vect);
	else return get_from_short_id_vector_binary(mid+1,end,id,vect);
}

template<class T> int16_t get_index_from_short_id_vector(int16_t id,svector<T> &vect)
{
	return get_index_from_short_id_vector_binary(0,(int16_t)vect.size()-1,id,vect);
}

template<class T> int16_t get_index_from_short_id_vector_binary(int16_t start,int16_t end,int16_t id,svector<T> &vect)
{
	if(start>end)return -1;

	int16_t mid=(start+end)>>1;

	T cptr=vect[mid];
	if(cptr->short_id==id)
		{
		return mid;
		}

	if(cptr->short_id>id)return get_index_from_short_id_vector_binary(start,mid-1,id,vect);
	else return get_index_from_short_id_vector_binary(mid+1,end,id,vect);
}
