#include "builtins.hh"
#include "download.hh"

namespace nix {

void builtinFetchurl(const BasicDerivation & drv)
{
    auto url = drv.env.find("url");
    if (url == drv.env.end()) throw Error("attribute ‘url’ missing");

    /* No need to do TLS verification, because we check the hash of
       the result anyway. */
    DownloadOptions options;
    options.verifyTLS = false;

    /* Show a progress indicator, even though stderr is not a tty. */
    options.forceProgress = true;

    auto data = downloadFile(url->second, options);

    auto out = drv.env.find("out");
    if (out == drv.env.end()) throw Error("attribute ‘url’ missing");
    writeFile(out->second, data.data);

    auto executable = drv.env.find("executable");
    if (executable != drv.env.end() && executable->second == "1") {
        if (chmod(out->second.c_str(), 0755) == -1)
            throw SysError(format("making ‘%1%’ executable") % out->second);
    }
}

}