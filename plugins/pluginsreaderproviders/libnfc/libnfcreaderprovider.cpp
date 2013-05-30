/**
 * \file libnfcreaderprovider.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Reader Proviser for LibNFC.
 */

#include "libnfcreaderprovider.hpp"

#ifdef __linux__
#include <stdlib.h>
#include <sys/time.h>
#endif

#include <sstream>

#include <iomanip>

#include "libnfcreaderunit.hpp"


namespace logicalaccess
{
	LibNFCReaderProvider::LibNFCReaderProvider() :
		ReaderProvider()
	{
		d_scc = 0;
		long scres = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &d_scc);
		if (scres != SCARD_S_SUCCESS)
		{
			char tmpbuf[128];
			memset(tmpbuf, 0x00, sizeof(tmpbuf));
			sprintf(tmpbuf, "Can't establish the context for PC/SC service (%x).", static_cast<unsigned int>(scres));
			THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException, tmpbuf);
		}
	}

	LibNFCReaderProvider::~LibNFCReaderProvider()
	{
		if (d_scc != 0)
		{
			SCardReleaseContext(d_scc);

			d_scc = 0;
		}
	}

	boost::shared_ptr<LibNFCReaderProvider> LibNFCReaderProvider::createInstance()
	{
		boost::shared_ptr<LibNFCReaderProvider> ret(new LibNFCReaderProvider());
		ret->refreshReaderList();
		return ret;
	}

	bool LibNFCReaderProvider::refreshReaderList()
	{
		bool r = false;
		d_system_readers.clear();

		DWORD rdlen = 0;
		if (SCARD_S_SUCCESS == SCardListReaders(d_scc, NULL, (char*)NULL, &rdlen))
		{
			char* rdnames = new char[rdlen];
			if (SCARD_S_SUCCESS == SCardListReaders(d_scc, NULL, rdnames, &rdlen))
			{
				char* rdname = rdnames;

				while (rdname[0] != '\0')
				{
					size_t f = strlen(rdname);
					std::string t(rdname, f);
					boost::shared_ptr<PCSCReaderUnit> unit = PCSCReaderUnit::createPCSCReaderUnit(t);
					unit->setReaderProvider(boost::weak_ptr<ReaderProvider>(shared_from_this()));
					d_system_readers.push_back(unit);

					rdname += strlen(rdname) + 1;
				}

				r = true;
			}
			delete[] rdnames;
		}

		return r;
	}

	boost::shared_ptr<ReaderUnit> LibNFCReaderProvider::createReaderUnit()
	{
		boost::shared_ptr<LibNFCReaderUnit> ret;
		ret.reset(new LibNFCReaderUnit(""));
		ret->setReaderProvider(boost::weak_ptr<ReaderProvider>(shared_from_this()));

		return ret;
	}
}

