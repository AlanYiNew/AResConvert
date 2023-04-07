# AResConvert. C++ Game Resource Converter (Meant to be cross platform cause all libraries support cross platform)
## General
### 1. Suport BrowserMode (GUI) ResourceConvert from xlxs to binary by defining protocol/*.proto in protobuf
### 2. BrowserMode: Convert from xlsx to binary format
### 3. CommandMode: Generate Resource Structure defined by protobuf. All generated structures are pods.
### 4. Support uploading to oss bucket

## TODO
### 1. Does not support compile the GUI interface under linux and apple
### 2. Encapsulate easy-to-use interface to read from binary format data. 
### 3. Determine data validation method. (MetaData md5 comparison)
### 4. Imporve format and style of code

## Dependecy
1. protobuf
2. OpenXlsx
3. inja
4. nlohmann
5. aliyun-oss-cpp-sdk

|              | Windows  | Linux | Apple |      
|  ----        | -------  | ------| ----- |
| BorwserMode  | Support  |  TODO | TODO  |
| CommandMode  | Support  |  TODO | TODO  |


## setup.json
    {
        "folders": ["protocol"], 
        "files": ["./protocol/table.proto"],
        "xlsx_folder" : "xlsx",
        "output_folder" : "bin",
        "convert" : "json",
        "oss_end_point": "<oss-end-point | optional, can be just empty string>",
        "bucket_name": "<oss-bucket-name | optional, can be just empty string>",
        "cloud_directory": "<directory-you-want-to-upload-in-the-bucket | optional, can be just empty string>",
        "access_key_id": "<oss-access-key-id | optional, can be just empty string>d",
        "access_key_secret": "<oss-access-key-secret | optional, can be just empty string>"
    }
       
