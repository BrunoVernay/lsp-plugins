/*
 * LSPTextDataSource.cpp
 *
 *  Created on: 21 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        static const char * const mimes[] =
        {
            "UTF8_STRING",
            "text/plain;charset=utf-8",
            "text/plain;charset=UTF-16LE",
            "text/plain;charset=UTF-16BE",
            "text/plain;charset=US-ASCII",
            "text/plain"
        };
        
        LSPTextDataSource::LSPTextDataSource(): IDataSource(mimes)
        {
        }
        
        LSPTextDataSource::~LSPTextDataSource()
        {
            sText.truncate();
        }

        io::IInStream   *LSPTextDataSource::open(const char *mime)
        {
            // Scan supported MIME types
            ssize_t idx = -1, i=0;
            for (const char *const *p = mimes; *p != NULL; ++p, ++i)
            {
                if (!::strcasecmp(mimes[i], mime))
                {
                    idx = i;
                    break;
                }
            }

            // Analyze found MIME type
            void *data      = NULL;
            size_t bytes    = 0;
            switch (idx)
            {
                case 0: // UTF8_STRING
                case 1: // text/plain;charset=utf-8
                    data = sText.clone_utf8(&bytes);
                    break;
                case 2: // text/plain;charset=UTF-16LE
                    data = __IF_LEBE(
                            sText.clone_utf16(&bytes),
                            sText.clone_native(&bytes, "UTF16-LE")
                        );
                    break;
                case 3: // text/plain;charset=UTF-16BE
                    data = __IF_LEBE(
                            sText.clone_native(&bytes, "UTF16-BE"),
                            sText.clone_utf16(&bytes)
                        );
                    break;
                case 4:
                    data = sText.clone_ascii(&bytes);
                    break;
                case 5:
                    data = sText.clone_native(&bytes);
                    break;
                default:
                    break;
            }

            // Format not supported?
            if (data == NULL)
                return NULL;

            // TODO: Create input stream and return pointer to the stream
            return NULL;
        }
    
    } /* namespace tk */
} /* namespace lsp */
