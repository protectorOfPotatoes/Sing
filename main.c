/*
*******************************************************************************************************
 _______  ___   __    _  _______ 
|       ||   | |  |  | ||       | ->MAINTAINER : Aditya Kumar Singh									
|  _____||   | |   |_| ||    ___| 	=>Voice Automated AI Chatbot powered by ChatGPT (OpenAI) API																		
| |_____ |   | |       ||   | __   															
|_____  ||   | |  _    ||   ||  |  																
 _____| ||   | | | |   ||   |_| |																
|_______||___| |_|  |__||_______|         																
    
*******************************************************************************************************
*/

extern const char* API_KEY;

#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#define LENGTH 1000

//callback function -> for testing it will just print the data on the next phase
//it will pass the contents to the next file using fprintf

size_t callback(void* contents, size_t size, size_t nrecdata, /*@unused@*/ void* ignorethis)

{
	size_t net_size = size * nrecdata;
	char* data = (char*)malloc(net_size + 1); // allocate one more byte for null-terminator
	if(data == NULL) 
	{
		fprintf(stderr, "Failed to allocate Data variable");
		return 1;	
		
	}
					    
	
	data[net_size] = '\0';
	memcpy(data, contents, net_size);

	printf("%s/n" , data);
	free(data);
	return net_size;
}

int main(void)
{	
	char text[LENGTH + 1];
	FILE* file = fopen("audio.txt", "r");
	if(file == NULL) {fprintf(stderr, "FileOpenError!");}
	fgets(text, LENGTH , file);
	fclose(file);
    //remove newline character and replace it with null terminator just in case	
	size_t len = strlen(text);
	if(len > 0 && text[len - 1] == '\n')
	{
		text[len - 1] = '\0';	
	}

/*-----------------------------------------------------------------------------------------------------*/

    // Initialization
	CURL *curl = curl_easy_init();
    
    // Error handling
	if (!curl)
    {
	fprintf(stderr, "InitializationError!");
        return 1;
    }
    
    //MAIN CODE(1) -> Set options
 
   if (curl)
   {
	
	cJSON *root = cJSON_CreateObject();
	if(root == NULL) {fprintf(stderr, "cJSON failed to create JSON object");}
	cJSON_AddStringToObject(root, "prompt", text);
	cJSON_AddNumberToObject(root, "max_tokens", 1000);
	cJSON_AddNumberToObject(root, "temperature", 0);

	const char* json_string = (char*)cJSON_Print(root);	


	struct curl_slist* header = NULL;

	char auth_header[256];
	snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", API_KEY);
	
	header = curl_slist_append(header, auth_header);
	header = curl_slist_append(header, "Content-Type: application/json");
	
	if(header == NULL) 
	{
		fprintf(stderr, "could not append params to header variable");
		return 1;
	}


	curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/engines/text-davinci-003/completions");//api call
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header); // request header
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_string); //request body

	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL); /*basically we're not using the ignorethis param in
								 callback() so this sets the ignorethis to NULL*/        

		
	cJSON_Delete(root);
   }
    
    //MAIN CODE(2) -> Perform previously set options
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK)
    {
        fprintf(stderr, "CurlPerformError: %s\n", curl_easy_strerror(result));
	return 1;
    }
    
	curl_easy_cleanup(curl);
	return 0;
}

