#include <string>
#include <boost/shared_ptr.hpp>
#include "logicalaccess/readerproviders/readerprovider.hpp"
#include "proxchip.hpp"

#ifdef _MSC_VER
#include "logicalaccess/msliblogicalaccess.h"
#else
#ifndef LIBLOGICALACCESS_API
#define LIBLOGICALACCESS_API
#endif
#ifndef DISABLE_PRAGMA_WARNING
#define DISABLE_PRAGMA_WARNING /**< \brief winsmcrd.h was modified to support this macro, to avoid MSVC specific warnings pragma */
#endif
#endif

extern "C"
{
	LIBLOGICALACCESS_API char *getLibraryName()
	{
		return (char *)"Prox";
	}
	
	LIBLOGICALACCESS_API void getProxChip(boost::shared_ptr<logicalaccess::Chip>* chip)
	{
		if (chip != NULL)
		{
			*chip = boost::shared_ptr<logicalaccess::ProxChip>(new logicalaccess::ProxChip());
		}
	}

	LIBLOGICALACCESS_API bool getChipInfoAt(unsigned int index, char* chipname, size_t chipnamelen, void** getterfct)
	{
		bool ret = false;
		if (chipname != NULL && chipnamelen == PLUGINOBJECT_MAXLEN && getterfct != NULL)
		{
			switch (index)
			{
			case 0:
				{
					*getterfct = (void*)&getProxChip;
					sprintf(chipname, CHIP_PROX);
					ret = true;
				}
				break;
			}
		}

		return ret;
	}
}
