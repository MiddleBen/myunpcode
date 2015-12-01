/**
 * 测试一下CURLOPT_WRITEDATA到底是怎么回事
 */
#include <stdio.h>
#include <curl/curl.h>
#include <error.h>
#include <string.h>

int main() {
        CURL * m_curl = curl_easy_init();
        if (m_curl == NULL) {
                perror("can not init libcurl!\n");
                return 1;
        }
        char *ivfsUrl = "http://10.100.77.237:8081/iVFS/ap/dsp/vendoroldProcess/cancelDetailList?id=1&pageSize=54";
        curl_easy_setopt(m_curl, CURLOPT_URL, ivfsUrl); // just set the url and see what happen.
        CURLcode res = curl_easy_perform(m_curl);
        if (CURLE_OK != res) {
                perror("curl perform error!\n");
                return -1;
        }
        return 0;
}
