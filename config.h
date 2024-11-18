#ifndef CONFIG_H
#define CONFIG_H

#ifdef NDEBUG 
#define DllAPI __declspec(dllexport)
#else
#define DllAPI
#endif

#define ORT_ABORT_ON_ERROR(expr, g_ort)                      \
  do {                                                       \
    OrtStatus* onnx_status = (expr);                         \
    if (onnx_status != NULL) {                               \
      const char* msg = g_ort->GetErrorMessage(onnx_status); \
      fprintf(stderr, "Error: %s\n", msg);                   \
      g_ort->ReleaseStatus(onnx_status);                     \
      return -1;                                             \
    }                                                        \
  } while (0)


#endif // CONFIG_H