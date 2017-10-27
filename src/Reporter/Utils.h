#ifndef UTILS_H
#define UTILS_H



#ifdef _DEBUG
#define _develop
#endif

#ifndef _develop
#define EM_TRY try
#else
#define EM_TRY 
#endif

#ifndef _develop 
#define EM_CATCHEND 
#else
#define EM_CATCHEND }
#endif

#ifndef _develop
#define EM_SCATCH(x) catch(x)
#else
#define EM_SCATCH(x) while(0){x; 
#endif

#ifndef _develop
#define EM_CATCH(x) catch(x)
#else
#define EM_CATCH(x) while(0){
#endif

#ifndef _develop
#define EM_THROW(x) {throw(x);}
#else
#define EM_THROW(x) {}
#endif

#define SAFEDELETE(p) if(p){delete p; p=NULL;}
#define SAFEFREE(p) if(p){free(p); p=NULL;}
#define SAFEDELETEEX(p) if(p){delete [] p; p=NULL;}

#endif