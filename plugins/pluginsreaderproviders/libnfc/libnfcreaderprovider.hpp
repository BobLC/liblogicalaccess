/**
 * \file libnfcreaderprovider.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC card reader provider. 
 */

#ifndef LOGICALACCESS_READERLIBNFC_PROVIDER_HPP
#define LOGICALACCESS_READERLIBNFC_PROVIDER_HPP

#include "logicalaccess/readerproviders/readerprovider.hpp"
#include "libnfcreaderunit.hpp"

#include <string>
#include <vector>

#include "logicalaccess/logs.hpp"

namespace logicalaccess
{	
	#define READER_LIBNFC		"LibNFC"

	/**
	 * \brief LibNFC Reader Provider class.
	 */
	class LIBLOGICALACCESS_API LibNFCReaderProvider : public ReaderProvider
	{
		protected:

			/**
			 * \brief Constructor.
			 */
			LibNFCReaderProvider();

		public:

			/**
			 * \brief Destructor.
			 */
			~LibNFCReaderProvider();

			/**
			 * \brief Create a new LibNFC reader provider instance.
			 * \return The LibNFC reader provider instance.
			 */
			static boost::shared_ptr<LibNFCReaderProvider> createInstance();

			/**
			 * \brief Get the reader provider type.
			 * \return The reader provider type.
			 */
			virtual std::string getRPType() const { return READER_LIBNFC; };

			/**
			 * \brief Get the reader provider name.
			 * \return The reader provider name.
			 */
			virtual std::string getRPName() const { return "NFC Devices (LibNFC)"; };

			/**
			 * \brief List all readers of the system.
			 * \return True if the list was updated, false otherwise.
			 */
			virtual bool refreshReaderList();

			/**
			 * \brief Get reader list for this reader provider.
			 * \return The reader list.
			 */
			virtual const ReaderList& getReaderList() { return d_system_readers; };						

			/**
			 * \brief Create a new reader unit for the reader provider.
			 * \return A reader unit.
			 */
			virtual boost::shared_ptr<ReaderUnit> createReaderUnit();



			/**
			 * \brief Get the NFC context.
			 * \return The NFC context.
			 */
			nfc_context* getContext() { return d_context; };


		protected:			

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

			/**
			 * \brief The system readers list.
			 */
			ReaderList d_system_readers;

#ifdef _MSC_VER
#pragma warning(pop)
#endif

			/**
			 * \brief The context.
			 */
			nfc_context* d_context;
	};

}

#endif /* LOGICALACCESS_READERLIBNFC_PROVIDER_HPP */

