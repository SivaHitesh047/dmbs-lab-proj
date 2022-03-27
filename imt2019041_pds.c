#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#include "pds.h"
#include "bst.h"

struct PDS_RepoInfo repo_handle;


int pds_create(char *repo_name) 
{
  char filename[30], indexfile[30];
  strcpy(filename,repo_name);
  strcpy(indexfile,repo_name);
  strcat(filename,".dat");
  strcat(indexfile,".ndx");
  FILE *fp = fopen(filename,"wb+");
  FILE *ifp = fopen(indexfile,"wb+");
  if(fp  == NULL || ifp == NULL) return PDS_FILE_ERROR;\
  fclose(fp);
  fclose(ifp);
  
  return PDS_SUCCESS;
}


int pds_open(char* repo_name, int rec_size)
{
    //check repo status
    if(repo_handle.repo_status == PDS_REPO_ALREADY_OPEN)
          return PDS_REPO_ALREADY_OPEN;
    
    // Open the data file and index file in rb+ mode
    char repo_dup[34];
    char repo_name_dup[34];
    strcpy(repo_name_dup, repo_name);
    strcpy(repo_dup, repo_name);
    strcat(repo_name,".dat");
    strcat(repo_dup,".ndx");

    // Update the fields of PDS_RepoInfo appropriately
    repo_handle.pds_data_fp = fopen(repo_name,"rb+");
    repo_handle.pds_ndx_fp = fopen(repo_dup,"ab+");
    repo_handle.repo_status = PDS_REPO_OPEN;
    repo_handle.rec_size = rec_size;
    repo_handle.pds_bst = NULL;
    strcpy(repo_handle.pds_name,repo_name_dup);

    // Build BST and store in pds_bst by reading index entries from the index file
    int iErr = pds_load_ndx();
    if(iErr == PDS_LOAD_NDX_FAILED)
      return PDS_LOAD_NDX_FAILED;

    // Close only the index file
    fclose(repo_handle.pds_ndx_fp);

    return PDS_SUCCESS;
}

int pds_load_ndx()
{
  // Internal function used by pds_open to read index entries into BST
  fseek(repo_handle.pds_ndx_fp, 0, SEEK_END);
  int end = ftell(repo_handle.pds_ndx_fp);
  fseek(repo_handle.pds_ndx_fp,0,SEEK_SET);

  while(ftell(repo_handle.pds_ndx_fp) != end){
    struct PDS_NdxInfo *ndxInfo = malloc(sizeof(struct PDS_NdxInfo));
    int fErr = fread(ndxInfo,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
    if(fErr!=1)
      return PDS_LOAD_NDX_FAILED;

    int bErr = bst_add_node(&(repo_handle.pds_bst),ndxInfo->key,ndxInfo);
    if(bErr!=BST_SUCCESS)
      return PDS_LOAD_NDX_FAILED;
  }
  
  return PDS_SUCCESS;
}

int put_rec_by_key(int key, void*rec)
{
  //check repo status
  if(repo_handle.repo_status == PDS_REPO_OPEN){
    // Seek to the end of the data file
    fseek(repo_handle.pds_data_fp,0,SEEK_END);
    // Create an index entry with the current data file location using ftell
    int offset = ftell(repo_handle.pds_data_fp);
    struct PDS_NdxInfo *ndxInfo = malloc(sizeof(struct PDS_NdxInfo));
    ndxInfo->key = key;
    ndxInfo->offset = offset;
    // (NEW) ENSURE is_deleted is set to 0 when creating index entry
    ndxInfo->is_deleted = 0;
    // Add index entry to BST using offset returned by ftell
    int bErr = bst_add_node(&(repo_handle.pds_bst),key,ndxInfo);
    if(bErr != BST_SUCCESS)
        return PDS_ADD_FAILED;
    // Write the key at the current data file location
    int keyWriteSuccess = fwrite(&key,sizeof(int),1,repo_handle.pds_data_fp);
    // Write the record after writing the key
    int recWriteSuccess = fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);

    if(keyWriteSuccess == 1 && recWriteSuccess == 1)
      return PDS_SUCCESS;
      
    else
      return PDS_ADD_FAILED;
  }
  
  else
    return PDS_REPO_NOT_OPEN;
}


int get_rec_by_ndx_key(int key, void*rec)
{
  //check repo status
  if(repo_handle.repo_status == PDS_REPO_OPEN){
    // Search for index entry in BST
    struct BST_Node *node = bst_search(repo_handle.pds_bst,key);
    
    if(node == NULL)
      return PDS_REC_NOT_FOUND;
    // (NEW) Check if the entry is deleted and if it is deleted, return PDS_REC_NOT_FOUND
    struct PDS_NdxInfo *ndxInfo = node->data;
    if(ndxInfo->is_deleted==1)
      return PDS_REC_NOT_FOUND;
    // Seek to the file location based on offset in index entry
    fseek(repo_handle.pds_data_fp,ndxInfo->offset,SEEK_SET);
    int k;
    int kErr = fread(&k,sizeof(int),1,repo_handle.pds_data_fp);
    // Read the record after reading the key
    int rErr = fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);

    if(rErr != 1 || kErr != 1)
      return PDS_REC_NOT_FOUND;

    else
      return PDS_SUCCESS;
  }

  else
    return PDS_REPO_NOT_OPEN; 
}

int unloadPreOrder(struct BST_Node *node){
  if(node == NULL)
    return PDS_SUCCESS;

  struct PDS_NdxInfo *ndxInfo = node->data;
  int err;

  //printf("%d %d",ndxInfo->key,ndxInfo->is_deleted);
  if(ndxInfo->is_deleted != 1)
  {
    err = fwrite(ndxInfo,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
    if(err != 1)
      return PDS_NDX_SAVE_FAILED;
  }

  int lErr = unloadPreOrder(node->left_child);
  int rErr = unloadPreOrder(node->right_child);

  if(lErr != PDS_SUCCESS || rErr != PDS_SUCCESS)
    return PDS_NDX_SAVE_FAILED;
  
  return PDS_SUCCESS;
}


int pds_close() 
{
  //check repo status
  if(repo_handle.repo_status == PDS_REPO_OPEN){
    // Open the index file in wb mode (write mode, not append mode)
    char repo_dup[34];
    strcpy(repo_dup, repo_handle.pds_name);
    strcat(repo_dup,".ndx"); 
    repo_handle.pds_ndx_fp = fopen(repo_dup,"wb");
 
    // Unload the BST into the index file by traversing it in PRE-ORDER (overwrite the entire index file)
    // (NEW) Ignore the index entries that have already been deleted. 
    int iErr = unloadPreOrder(repo_handle.pds_bst);
    
    if(iErr == PDS_NDX_SAVE_FAILED)
      return PDS_NDX_SAVE_FAILED;

    // Free the BST by calling bst_destroy()
    bst_destroy(repo_handle.pds_bst);
    // Close the index file and data file
    fclose(repo_handle.pds_ndx_fp);
    fclose(repo_handle.pds_data_fp);
    
    // Update the fields of PDS_RepoInfo appropriately
    repo_handle.repo_status = PDS_REPO_CLOSED;
    repo_handle.pds_data_fp = NULL;
    repo_handle.pds_ndx_fp = NULL;
    repo_handle.pds_bst = NULL;
    strcpy(repo_handle.pds_name,"");
  }

  else
    return PDS_REPO_NOT_OPEN;

  return  PDS_SUCCESS;
}

int get_rec_by_non_ndx_key(void *key, void *rec, int (*matcher)(void *rec, void *key), int *io_count)
{
  if(repo_handle.repo_status==PDS_REPO_OPEN){
    // Seek to beginning of file
    fseek(repo_handle.pds_data_fp,0,SEEK_SET);
    int key_dup;
    
    *io_count=0;
    // Perform a table scan - iterate over all the records
    while(!feof(repo_handle.pds_data_fp))
    {
      //Read the key and the record
      int kErr = fread(&key_dup,sizeof(int),1,repo_handle.pds_data_fp);
      int rErr = fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
      
      if(kErr!=1 || rErr!=1)
        return PDS_REC_NOT_FOUND;
      //Increment io_count by 1 to reflect count no. of records read
      *io_count += 1;
      //Use the function in function pointer to compare the record with required key
      int match = matcher(rec,key);
      //(NEW) Check the entry of the record in the BST and see if it is deleted. If so, return PDS_REC_NOT_FOUND
      
      
      if(match==0)
      {
        struct BST_Node *node = bst_search(repo_handle.pds_bst,key_dup);
        struct PDS_NdxInfo *info = node->data;
        int isDel = info->is_deleted;
        if(isDel==1)
          return PDS_REC_NOT_FOUND;

        return PDS_SUCCESS;
      }
        
      else if(match>1)
        return PDS_REC_NOT_FOUND;
    }

    return PDS_REC_NOT_FOUND;
  }

  else
    return PDS_REPO_NOT_OPEN;
}

int delete_rec_by_ndx_key( int key) // New Function
{
  if(repo_handle.repo_status==PDS_REPO_OPEN){
    // Search for the record in the BST using the key
    struct BST_Node *node = bst_search(repo_handle.pds_bst,key);
    // If record not found, return PDS_DELETE_FAILED
    if(node==NULL)
      return PDS_DELETE_FAILED;

    // If record is found, check if it has already been deleted, if so return PDS_DELETE_FAILED  
    struct PDS_NdxInfo *info = node->data;
    int isDel = info->is_deleted;
    if(isDel==1)
      return PDS_DELETE_FAILED;
    // Else, set the record to deleted and return PDS_SUCCESS
    else
    {
      info->is_deleted = 1;
      return PDS_SUCCESS;
    }
  }

  else
    return PDS_REPO_NOT_OPEN;
}

int pds_overwrite( int key, void *rec )
{
  if(repo_handle.repo_status==PDS_REPO_OPEN){
    // Search for the record in the BST using the key
    struct BST_Node *node = bst_search(repo_handle.pds_bst,key);
    // If record not found, return PDS_UPDATE_FAILED
    if(node==NULL)
      return PDS_UPDATE_FAILED;
    
    struct PDS_NdxInfo *info = node->data;
    int isDel = info->is_deleted;
    if(isDel==1)
      return PDS_UPDATE_FAILED;

    
    fseek(repo_handle.pds_data_fp,info->offset,SEEK_SET);
    int k;
    int kErr = fread(&k,sizeof(int),1,repo_handle.pds_data_fp);
    
    if(kErr != 1)
      return PDS_UPDATE_FAILED;
    // Overwrite the record after reading the key
    int rErr = fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
    if(rErr!=1)
      return PDS_UPDATE_FAILED;

    return PDS_SUCCESS;
    
  }

  else
    return PDS_REPO_NOT_OPEN;
}
