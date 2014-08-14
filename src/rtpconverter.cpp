/*============================================================================
 *Name        : rtpconverter.cpp
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : the RTPConverter class can convert media data between srtp and rtp
 *============================================================================
 */
#include "rtpconverter.h"
#include "rtpdefine.h"
#include "commontype.h"
#include <string>

#include <cstdio>
#include <iostream>
using namespace std;

RTPConverter::RTPConverter(){
	this->ready = false;
}

RTPConverter::~RTPConverter(){
	cout << "in rtpConverter dtor.." << endl;

	if(this->ready == true){
		srtp_dealloc(sessionDecrypt);
		srtp_dealloc(sessionEncrypt);
	}
	cout << "finished in rtpConverter deleted." << endl;
}

int RTPConverter::init(string enkey, string dekey){
	cout << "init rtp converter in RTPConverter::init!\n"<< endl;

	memset(masen, 0, sizeof(masen));
	string enc = base64_decode(enkey);
	memcpy(masen, enc.c_str(), (unsigned int)enc.size());

	crypto_policy_set_aes_cm_128_hmac_sha1_80(&policyEncrypt.rtp);
	crypto_policy_set_aes_cm_128_hmac_sha1_80(&policyEncrypt.rtcp);
	policyEncrypt.ssrc.type = ssrc_any_outbound;
	policyEncrypt.key = masen;
	policyEncrypt.window_size = 1024;
	policyEncrypt.allow_repeat_tx = 1;
	policyEncrypt.next = NULL;

	err_status_t encryptStatus = srtp_create(&sessionEncrypt, &policyEncrypt);
	if(encryptStatus != err_status_ok){
		printf("srtp_create encrypt failed : %d\n", encryptStatus);
		return encryptStatus;
	}else{
		printf("srtp_create encrypt success\n");
	}


	memset(masterKey, 0, sizeof(masterKey));
	string dec = base64_decode(dekey);
	memcpy(masterKey, dec.c_str(), (unsigned int)dec.size());
	crypto_policy_set_aes_cm_128_hmac_sha1_80(&policyDecrypt.rtp);
	crypto_policy_set_aes_cm_128_hmac_sha1_80(&policyDecrypt.rtcp);
	policyDecrypt.ssrc.type = ssrc_any_inbound;
	policyDecrypt.key = masterKey;
	policyDecrypt.window_size = 1024;
	policyDecrypt.allow_repeat_tx = 1;
	policyDecrypt.next = NULL;

	err_status_t decryptStatus = srtp_create(&sessionDecrypt, &policyDecrypt);
	if(decryptStatus != err_status_ok){
		printf("srtp_create decrypt failed : %d\n", decryptStatus);
		return decryptStatus;
	}else{
		printf("srtp_create decrypt success\n");
	}

	ready = true;
	return err_status_ok;
}

//return 0 on success
int RTPConverter::encryptData(unsigned char *buffer, int &len) {
	err_status_t status = srtp_protect(sessionEncrypt, buffer, &len);
	if (status != err_status_ok) {
		printf("srtp_protect error code : status=%d\n", status);
	}
	return status;
}

//return 0 on success
int RTPConverter::decryptData(unsigned char *buffer, int &len) {
	err_status_t status = srtp_unprotect(sessionDecrypt, buffer, &len);
	if (status != err_status_ok) {
		printf("srtp_unprotect error code : status=%d\n", status);
	}
	return status;
}

int RTPConverter::encryptRTCP(unsigned char* buffer, int &len){
	err_status_t status = srtp_protect_rtcp(sessionEncrypt, buffer, &len);
	if(status != err_status_ok){
		printf("srtp_protect_rtcp error code: status=%d\n", status);
	}
	return status;
}
