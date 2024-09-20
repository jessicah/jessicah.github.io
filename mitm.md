# Using MITM Proxy with Android Certificate Pinning

Using https://github.com/shroudedcode/apk-mitm, it can automatically
decompile, patch, and recompile to remove certificate pinning, allowing
`mitmproxy` to successfully intercept android app traffic.

In some cases, the resources end up as raw integer values instead of
their enum values.

This can be fixed by adding them to `attrs.xml`, and updating `styles.xml`
to use the names added to `attrs.xml`.

Pages about how to export and use the SSL secrets:

- https://docs.mitmproxy.org/stable/howto-wireshark-tls/
- https://docs.mitmproxy.org/stable/concepts-certificates/
- https://wiki.wireshark.org/TLS#using-the-pre-master-secret
- https://github.com/TrackerControl/JustTrustMe

And setting an environment variable (temporarily) in Powershell:

`$env:SSLKEYLOGFILE = <path to store>`
