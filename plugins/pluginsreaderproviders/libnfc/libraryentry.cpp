#include <string>
#include <boost/shared_ptr.hpp>
#include "logicalaccess/readerproviders/readerprovider.hpp"
#include "libnfcreaderprovider.hpp"

#ifdef _MSC_VER
#include "logicalaccess/msliblogicalaccess.h"
#else
#ifndef LIBLOGICALACCESS_API
#define LIBLOGICALACCESS_API
#endif
#endif

extern "C"
{
	LIBLOGICALACCESS_API char *getLibraryName()
	{
		return (char *)"LibNFC";
	}

	LIBLOGICALACCESS_API void getLibNFCReader(boost::shared_ptr<logicalaccess::ReaderProvider>* rp)
	{
		if (rp != NULL)
		{
			*rp = logicalaccess::LibNFCReaderProvider::createInstance();
		}
	}

	LIBLOGICALACCESS_API bool getReaderInfoAt(unsigned int index, char* readername, size_t readernamelen, void** getterfct)
	{
		bool ret = false;
		if (readername != NULL && readernamelen == PLUGINOBJECT_MAXLEN && getterfct != NULL)
		{
			switch (index)
			{
			case 0:
				{
					*getterfct = (void*)&getLibNFCReader;
					sprintf(readername, READER_PCSC);
					ret = true;
				}
				break;
			}
		}

		return ret;
	}
}
