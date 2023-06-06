#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

#define LENGTH 1000

//callback function -> for testing it will just print the data on the next phase
//it will pass the contents to the next file

size_t callback(void* contents, size_t size, size_t nrecdata, /*@unused@*/ void* ignorethis)

{
	size_t net_size = size * nrecdata;
	char* data = (char*)malloc(net_size + 1); // allocate one more byte for null-terminator
	if(data == NULL) 
	{
		fprintf(stderr, "Failed to allocate Data variable");
	}
					    
	
	data[net_size] = '\0';
	memcpy(data, contents, net_size + 1);

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

/*---------------------------------------------------------------------------------------------------------------------------------------*/

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
	
	//i mademy life easier by using cJSON to interpret the json, so much more readble	
	cJSON *root = cJSON_CreateObject();
	if(root == NULL) {fprintf(stderr, "cJSON failed to create JSON object");}
        cJSON_AddStringToObject(root, "prompt", text);
        cJSON_AddNumberToObject(root, "temperature", 0);
        cJSON_AddNumberToObject(root, "max_tokens", 1000);
        cJSON_AddNumberToObject(root, "top_p", 1);
        cJSON_AddNumberToObject(root, "frequency_penalty", 0.0);
        cJSON_AddNumberToObject(root, "presence_penalty", 0.0);
	
	char* json_string = cJSON_Print(root);

	char pass_data[strlen(json_string) + 1 + LENGTH];

	strcpy(pass_data, json_string);
	strcat(pass_data, "\0");
	struct curl_slist* header = NULL;

	
	header = curl_slist_append(header, "Authorization: Bearer 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");
	header = curl_slist_append(header, "Content-Type: application/json");
	
	if(header == NULL) 
	{
		fprintf(stderr, "could not append params to header variable");
	}


	curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/engines/text-davinci-003/completions");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pass_data);

	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL); //basically we're not using the ignorethis param in callback() so this sets the ignorethis to NULL
        
	free(json_string);
	cJSON_Delete(root);
    }
    
    //MAIN CODE(2) -> Perform previously set options
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK)
    {
        fprintf(stderr, "CurlPerformError: %s\n", curl_easy_strerror(result));
    }
    
	
	curl_easy_cleanup(curl);
	return 0;
}

