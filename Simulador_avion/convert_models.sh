#!/bin/bash
##############################################################
# Script: convert_models.sh
# Propósito: Convertir modelos Blender y FBX a OBJ
# Requisitos: Blender, unrar (opcional)
##############################################################

set -e  # Salir si hay error

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
MODELS_DIR="${SCRIPT_DIR}/models"
EXPORT_SCRIPT="${SCRIPT_DIR}/export_blend.py"

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'  # No Color

echo -e "${BLUE}========================================================${NC}"
echo -e "${BLUE}  Script de Conversión de Modelos 3D${NC}"
echo -e "${BLUE}========================================================${NC}"

# Verificar que exista el directorio de modelos
if [ ! -d "$MODELS_DIR" ]; then
    echo -e "${YELLOW}Directorio $MODELS_DIR no existe. Creando...${NC}"
    mkdir -p "$MODELS_DIR"
fi

# Verificar existencia de Blender
if ! command -v blender &> /dev/null; then
    echo -e "${YELLOW}⚠️  Blender no encontrado en PATH${NC}"
    echo "    Por favor instala Blender o agrégalo a PATH"
fi

# ============================================================
# 1. CONVERTIR ARCHIVOS BLEND A OBJ
# ============================================================
echo ""
echo -e "${BLUE}[1] Convirtiendo archivos BLEND a OBJ...${NC}"

convert_blend_to_obj() {
    local blend_file="$1"
    local obj_file="${blend_file%.blend}.obj"

    if [ -f "$obj_file" ]; then
        echo -e "${GREEN}✅ Ya existe: $(basename $obj_file)${NC}"
        return 0
    fi

    if [ ! -f "$blend_file" ]; then
        echo -e "${RED}❌ No encontrado: $(basename $blend_file)${NC}"
        return 1
    fi

    echo -e "${YELLOW}🔄 Convirtiendo: $(basename $blend_file)${NC}"

    if blender -b "$blend_file" -P "$EXPORT_SCRIPT" --no-window 2>/dev/null; then
        if [ -f "$obj_file" ]; then
            echo -e "${GREEN}✅ Convertido: $(basename $obj_file)${NC}"
            return 0
        else
            echo -e "${RED}❌ Error: Archivo de salida no creado${NC}"
            return 1
        fi
    else
        echo -e "${RED}❌ Error en Blender${NC}"
        return 1
    fi
}

BLEND_FILES=(
    "American Airlines Boeing 737-800.blend"
    "kawasaki-ki-61.blend"
)

for blend_file in "${BLEND_FILES[@]}"; do
    convert_blend_to_obj "$MODELS_DIR/$blend_file"
done

# ============================================================
# 2. DESCOMPRIMIR ARCHIVOS RAR
# ============================================================
echo ""
echo -e "${BLUE}[2] Descomprimiendo archivos RAR...${NC}"

if ! command -v unrar &> /dev/null; then
    echo -e "${YELLOW}⚠️  unrar no encontrado. Instalando...${NC}"
    if command -v apt-get &> /dev/null; then
        sudo apt-get install -y unrar
    elif command -v brew &> /dev/null; then
        brew install unrar
    else
        echo -e "${RED}❌ No se pudo instalar unrar. Por favor instálalo manualmente.${NC}"
    fi
fi

if [ -f "$MODELS_DIR/uploads_files_5625074_MS-406.rar" ]; then
    echo -e "${YELLOW}🔄 Descomprimiendo MS-406.rar...${NC}"
    if unrar x "$MODELS_DIR/uploads_files_5625074_MS-406.rar" "$MODELS_DIR/" -y &> /dev/null; then
        echo -e "${GREEN}✅ MS-406.rar descomprimido${NC}"

        # Buscar archivos OBJ dentro
        obj_files=$(find "$MODELS_DIR" -name "*.obj" -type f | grep -v "American Airlines" | grep -v kawasaki)
        if [ -z "$obj_files" ]; then
            echo -e "${YELLOW}ℹ️  Dentro del RAR encontrados:${NC}"
            unrar l "$MODELS_DIR/uploads_files_5625074_MS-406.rar" | grep -E "\.(obj|fbx|blend)$"
        fi
    else
        echo -e "${RED}❌ Error al descomprimir RAR${NC}"
    fi
else
    echo -e "${YELLOW}⚠️  No encontrado: uploads_files_5625074_MS-406.rar${NC}"
fi

# ============================================================
# 3. CONVERTIR FBX A OBJ (INTENTO)
# ============================================================
echo ""
echo -e "${BLUE}[3] Intentando convertir FBX a OBJ...${NC}"

if [ -f "$MODELS_DIR/MYSTERE IV N 117 Provence Nancy.fbx" ]; then
    obj_file="$MODELS_DIR/MYSTERE IV N 117 Provence Nancy.obj"

    if [ -f "$obj_file" ]; then
        echo -e "${GREEN}✅ Ya existe: MYSTERE IV N 117 Provence Nancy.obj${NC}"
    else
        echo -e "${YELLOW}ℹ️  FBX detectado. Necesita conversión.${NC}"
        echo -e "${YELLOW}    Opciones:${NC}"
        echo -e "${YELLOW}    1. Usar Blender: blender -b archivo.fbx -P export_blend.py${NC}"
        echo -e "${YELLOW}    2. Usar Assimp: assimp export archivo.fbx archivo.obj${NC}"
        echo -e "${YELLOW}    3. Convertir manualmente en Blender UI${NC}"
    fi
else
    echo -e "${YELLOW}⚠️  No encontrado: MYSTERE IV N 117 Provence Nancy.fbx${NC}"
fi

# ============================================================
# 4. REPORTE FINAL
# ============================================================
echo ""
echo -e "${BLUE}[4] Reporte de modelos disponibles:${NC}"
echo ""

if [ -d "$MODELS_DIR" ]; then
    echo -e "${BLUE}Archivos en ./models/:${NC}"
    ls -lh "$MODELS_DIR" | tail -n +2 | awk '{print "  " $9 " (" $5 ")"}'
fi

echo ""
echo -e "${BLUE}========================================================${NC}"
echo -e "${GREEN}✅ Procesamiento completado${NC}"
echo -e "${BLUE}========================================================${NC}"

echo ""
echo "Próximos pasos:"
echo "  1. Verificar que los archivos .obj existan en ./models/"
echo "  2. Compilar el proyecto incluyendo model_loader.h/cpp"
echo "  3. Ejecutar el simulador"
echo ""
