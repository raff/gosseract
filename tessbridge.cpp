#if __FreeBSD__ >= 10
#include "/usr/local/include/tesseract/baseapi.h"
#include "/usr/local/include/leptonica/allheaders.h"
#else
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#endif

#include "tessbridge.h"

TessBaseAPI Create() {
  tesseract::TessBaseAPI * api = new tesseract::TessBaseAPI();
  return (void*)api;
}

void Free(TessBaseAPI a) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  api->End();
  delete api;
}

void Clear(TessBaseAPI a) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  api->Clear();
}

void ClearPersistentCache(TessBaseAPI a) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  api->ClearPersistentCache();
}

int Init(TessBaseAPI a, char* tessdataprefix, char* languages, int oem) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  return api->Init(tessdataprefix, languages, (tesseract::OcrEngineMode)oem);
}

int Init(TessBaseAPI a, char* tessdataprefix, char* languages, char* configfilepath, int oem) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  if (configfilepath != NULL) {
    char *configs[]={configfilepath};
    int configs_size = 1;
    return api->Init(tessdataprefix, languages, (tesseract::OcrEngineMode)oem,  configs, configs_size, NULL, NULL, false);
  } else {
    return api->Init(tessdataprefix, languages, (tesseract::OcrEngineMode)oem);
  }
}

bool SetVariable(TessBaseAPI a, char* name, char* value) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  return api->SetVariable(name, value);
}

void SetPixImage(TessBaseAPI a, PixImage pix) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  Pix *image = (Pix*) pix;
  api->SetImage(image);
  if (api->GetSourceYResolution() < 70) {
    api->SetSourceResolution(70);
  }
}

void SetPageSegMode(TessBaseAPI a, int m) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  tesseract::PageSegMode mode = (tesseract::PageSegMode)m;
  api->SetPageSegMode(mode);
}

int GetPageSegMode(TessBaseAPI a) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  return api->GetPageSegMode();
}

char* UTF8Text(TessBaseAPI a) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  return api->GetUTF8Text();
}

char* HOCRText(TessBaseAPI a) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  return api->GetHOCRText(0);
}

bounding_boxes* GetBoundingBoxes(TessBaseAPI a, int pageIteratorLevel) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  struct bounding_boxes* box_array;
  box_array = (bounding_boxes*)malloc(sizeof(bounding_boxes));
  // linearly resize boxes array
  int realloc_threshold = 900;
  int realloc_raise = 1000;
  int capacity = 1000;
  box_array->boxes = (bounding_box*)malloc(capacity * sizeof(bounding_box));
  box_array->length = 0;
  api->Recognize(NULL);
  tesseract::ResultIterator* ri = api->GetIterator();
  tesseract::PageIteratorLevel level = (tesseract::PageIteratorLevel)pageIteratorLevel;

  if (ri != 0) {
    do {
      if ( box_array->length >= realloc_threshold ) {
        capacity += realloc_raise;
        box_array->boxes = (bounding_box*)realloc(box_array->boxes, capacity * sizeof(bounding_box));
        realloc_threshold += realloc_raise;
      }
      box_array->boxes[box_array->length].word = ri->GetUTF8Text(level);
      box_array->boxes[box_array->length].confidence = ri->Confidence(level);
      ri->BoundingBox(level, &box_array->boxes[box_array->length].x1, &box_array->boxes[box_array->length].y1, &box_array->boxes[box_array->length].x2, &box_array->boxes[box_array->length].y2);
      box_array->length++;
    } while (ri->Next(level));
  }

  return box_array;
}

const char* Version(TessBaseAPI a) {
  tesseract::TessBaseAPI * api = (tesseract::TessBaseAPI*)a;
  const char* v = api->Version();
  return v;
}

PixImage CreatePixImageByFilePath(char* imagepath) {
  Pix *image = pixRead(imagepath);
  return (void*)image;
}

PixImage CreatePixImageFromBytes(unsigned char* data, int size) {
  Pix *image = pixReadMem(data, (size_t)size);
  return (void*)image;
}


void DestroyPixImage(PixImage pix){
  Pix *img = (Pix*) pix;
  pixDestroy(&img);
}
