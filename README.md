# Audio Player

Reproductor de audio multiplataforma construido con **Qt6 / C++23**, compatible con Linux (desktop) y Android.

---

## Características

- Reproducción de archivos de audio en formatos MP3, WAV, OGG, FLAC, M4A, AAC y OPUS
- Carga de carpetas completas con ordenamiento alfabético automático
- Lista de reproducción con resaltado de pista activa
- Controles de reproducción: play/pause, stop, siguiente, anterior
- Salto rápido de ±10 segundos
- Barra de progreso con seek interactivo
- Control de volumen (solo desktop)
- Avance automático al terminar cada pista
- Comportamiento adaptado por plataforma: en Android usa el sistema de permisos y `DocumentFile` para acceder al almacenamiento

---

## Requisitos

### Desktop (Linux)

| Dependencia                     | Versión mínima    |
| ------------------------------- | ----------------- |
| CMake                           | 3.21              |
| Qt6 (Core, Widgets, Multimedia) | 6.x               |
| Compilador con soporte C++23    | GCC 13 / Clang 16 |

### Android

| Dependencia              | Versión mínima            |
| ------------------------ | ------------------------- |
| Qt6 para Android         | 6.x                       |
| Android NDK              | según `gradle.properties` |
| Android SDK (compileSdk) | según `build.gradle`      |
| Gradle                   | 8.x                       |

---

## Compilar

### Desktop (Linux)

```bash
cmake -B build -S .
cmake --build build
./build/bin/audioplayer
```

### Android

```bash
chmod +x build-android.sh
./build-android.sh
```

El script genera el APK usando `androiddeployqt` y Gradle. El APK firmado queda disponible en el directorio de salida configurado en `build-android.sh`.

---

## Permisos Android

| Permiso                 | Uso                              |
| ----------------------- | -------------------------------- |
| `READ_EXTERNAL_STORAGE` | Lectura de audio en Android ≤ 12 |
| `READ_MEDIA_AUDIO`      | Lectura de audio en Android ≥ 13 |

Los permisos se solicitan en tiempo de ejecución al iniciar la aplicación.
