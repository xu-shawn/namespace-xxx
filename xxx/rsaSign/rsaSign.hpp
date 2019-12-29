#pragma once

#include <iostream>
#include <openssl/rsa.h>  
#include <openssl/pem.h>  
#include <openssl/err.h>  
#include <openssl/bio.h>  
#include <openssl/evp.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/sha.h>  
#include <openssl/md5.h>  
#include <openssl/rand.h>  
#include <openssl/objects.h>  
#include <openssl/bio.h>
#include <openssl/des.h> 
#include <openssl/aes.h>
#include <openssl/evp.h>

namespace xxx
{
	static RSA* GetPublicKeyEx(char* filepath)
	{
		RSA* pubkey = RSA_new();

		BIO* pubio;

		pubio = BIO_new_file(filepath, "rb");
		pubkey = PEM_read_bio_RSAPublicKey(pubio, &pubkey, NULL, NULL);

		BIO_free(pubio);

		return pubkey;
	}

	static RSA* GetPrivateKeyEx(char* filepath)
	{
		RSA* prikey = RSA_new();

		BIO* priio;

		priio = BIO_new_file(filepath, "rb");
		prikey = PEM_read_bio_RSAPrivateKey(priio, &prikey, NULL, NULL);

		BIO_free(priio);

		return prikey;
	}

	static std::string base64encode(unsigned char* bindata, int binlength)
	{
		const char* base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		char* base64 = new char[(binlength + 1) * 2];
		memset(base64, 0, (binlength + 1) * 2);

		int i, j;
		unsigned char current;

		for (i = 0, j = 0; i < binlength; i += 3)
		{
			current = (bindata[i] >> 2);
			current &= (unsigned char)0x3F;
			base64[j++] = base64char[(int)current];

			current = ((unsigned char)(bindata[i] << 4)) & ((unsigned char)0x30);
			if (i + 1 >= binlength)
			{
				base64[j++] = base64char[(int)current];
				base64[j++] = '=';
				base64[j++] = '=';
				break;
			}
			current |= ((unsigned char)(bindata[i + 1] >> 4))& ((unsigned char)0x0F);
			base64[j++] = base64char[(int)current];

			current = ((unsigned char)(bindata[i + 1] << 2)) & ((unsigned char)0x3C);
			if (i + 2 >= binlength)
			{
				base64[j++] = base64char[(int)current];
				base64[j++] = '=';
				break;
			}
			current |= ((unsigned char)(bindata[i + 2] >> 6))& ((unsigned char)0x03);
			base64[j++] = base64char[(int)current];

			current = ((unsigned char)bindata[i + 2]) & ((unsigned char)0x3F);
			base64[j++] = base64char[(int)current];
		}
		base64[j] = '\0';
		std::string ret = "";
		ret = base64;

		delete[]base64;
		return ret;

	}

	static std::string RsaMd5Sign(const std::string& instr, const std::string pemfilepath)
	{
		unsigned char sign_value[1024]{ 0 };
		unsigned int sign_len;
		EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
		RSA* rsa = NULL; 
		EVP_PKEY* evpKey = NULL;  

		rsa = GetPrivateKeyEx((char*)pemfilepath.c_str());
		if (rsa == NULL)
		{
			printf("gen rsa err\r\n");
			return "";
		}
		evpKey = EVP_PKEY_new();  
		if (evpKey == NULL)
		{
			printf("EVP_PKEY_new err\n");
			RSA_free(rsa);
			return "";
		}
		if (EVP_PKEY_set1_RSA(evpKey, rsa) != 1) 
		{
			printf("EVP_PKEY_set1_RSA err\n");
			RSA_free(rsa);
			EVP_PKEY_free(evpKey);
			return "";
		}
		EVP_MD_CTX_init(mdctx);
		if (!EVP_SignInit_ex(mdctx, EVP_md5(), NULL))
		{
			printf("err\n");
			EVP_PKEY_free(evpKey);
			RSA_free(rsa);
			return "";
		}
		if (!EVP_SignUpdate(mdctx, instr.data(), instr.size()))
		{
			printf("err\n");
			EVP_PKEY_free(evpKey);
			RSA_free(rsa);
			return "";
		}
		if (!EVP_SignFinal(mdctx, sign_value, &sign_len, evpKey))
		{
			printf("err\n");
			EVP_PKEY_free(evpKey);
			RSA_free(rsa);
			return "";
		}
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		EVP_MD_CTX_free(mdctx);

		return base64encode(sign_value, sign_len);
	}



}//namespace xxx
