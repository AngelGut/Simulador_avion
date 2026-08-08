# 📁 Carpeta de Modelos 3D

## 📌 Instrucciones de Carga

Esta carpeta está configurada para **cargar automáticamente** cualquier archivo `.obj` o `.fbx` que se coloque aquí.

### ✅ Formatos Soportados
- ✅ **OBJ** - Wavefront (completamente soportado)
- ✅ **FBX** - Autodesk FBX (parcialmente soportado con SDK)
- ⚠️ **BLEND** - Blender (requiere conversión previa a OBJ)
- ⚠️ **RAR** - Archivo comprimido (se descomprime automáticamente)

## 🚀 Cómo Subir Modelos

### Opción 1: SCP (desde terminal)
```bash
# Subir un archivo OBJ
scp tu_modelo.obj usuario@servidor:/home/user/Simulador_avion/models/

# Subir múltiples archivos
scp *.obj usuario@servidor:/home/user/Simulador_avion/models/
```

### Opción 2: Git (si usas repositorio)
```bash
# Copiar archivo a la carpeta
cp tu_modelo.obj ./models/

# Agregar y hacer commit
git add models/
git commit -m "Agregar modelos 3D"
git push
```

### Opción 3: SFTP (con cliente gráfico)
- Usa FileZilla, WinSCP, o similar
- Conecta al servidor
- Navega a `/home/user/Simulador_avion/models/`
- Arrastra y suelta los archivos

### Opción 4: Directamente en Claude Code
- Usa la herramienta de upload de archivos si está disponible

## 📋 Mapeo de los 5 Modelos

Cuando subas los archivos, nómbralos así para que se identifiquen correctamente:

| Archivo a Subir | Nombre Original |
|---|---|
| `american_airlines_b737_800.obj` | American Airlines Boeing 737-800 |
| `kawasaki_ki61.obj` | Kawasaki Ki-61 |
| `ms406.obj` | MS-406 |
| `mystere_iv_provence.fbx` | MYSTERE IV N 117 Provence Nancy |
| `millennium_falcon.obj` | Millennium Falcon |

**Nota:** Los nombres no son críticos - el programa cargará cualquier `.obj` o `.fbx`

## 🔍 Verificación

Una vez subidos los archivos:

```bash
# Ver archivos en la carpeta
ls -la models/

# Deberías ver algo como:
# american_airlines_b737_800.obj
# kawasaki_ki61.obj
# millennium_falcon.obj
# etc.
```

## 🎮 Cómo se Cargan Automáticamente

El programa hace esto automáticamente al iniciar:

```cpp
// En main.cpp:
ModelManagerAuto& autoMgr = ModelManagerAuto::getInstance();
int loadedCount = autoMgr.loadAllModelsFromFolder("./models/");

std::cout << "Cargados " << loadedCount << " modelos\n";

// Listar modelos disponibles
autoMgr.printModelsInfo();
```

## 📊 Estructura Esperada Final

```
Simulador_avion/
├── models/
│   ├── american_airlines_b737_800.obj      ✅
│   ├── kawasaki_ki61.obj                   ✅
│   ├── ms406.obj                           ✅
│   ├── mystere_iv_provence.fbx             ✅
│   ├── millennium_falcon.obj               ✅
│   └── README.md                           (este archivo)
│
└── Simulador_avion/
    ├── main.cpp                            (integrado para cargar automático)
    ├── model_loader.h/cpp                  (sistema de carga)
    ├── model_manager_auto.h/cpp            (auto-detección)
    └── ...
```

## ⚡ Características

- 🔄 **Auto-detección** - Detecta automáticamente formatos soportados
- 📊 **Reporte** - Muestra cantidad de vértices y triángulos de cada modelo
- 🎯 **Sin configuración** - Solo copia y pega archivos
- 🔄 **Carga dinámica** - Puedes agregar/remover archivos sin recompilar

## 🛠️ Troubleshooting

### "No se cargan los modelos"
- ✅ Verifica que estén en `./models/`
- ✅ Verifica la extensión sea `.obj` o `.fbx` (minúscula)
- ✅ Verifica permisos: `chmod 644 models/*`

### "Error de permisos"
```bash
chmod 755 models/
chmod 644 models/*
```

### "FBX no se carga"
- FBX requiere librería Assimp o FBX SDK
- Alternativa: Convierte a OBJ primero
- Ver: `Arq004_preparacion_modelos_3d.md`

---

**Estado:** ✅ Carpeta lista para recibir modelos
**Última actualización:** 2025-07-23
