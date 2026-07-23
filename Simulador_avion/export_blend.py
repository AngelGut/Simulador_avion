#!/usr/bin/env python3
"""
Script para exportar modelos Blender a formato OBJ.
Diseñado para usarse con: blender -b archivo.blend -P export_blend.py --no-window

Uso:
    blender -b modelo.blend -P export_blend.py --no-window

Genera: modelo.obj en el mismo directorio
"""

import bpy
import os
import sys

def export_to_obj():
    """Exporta el archivo Blender activo a OBJ."""

    # Obtener ruta del archivo Blender
    blend_file = bpy.data.filepath

    if not blend_file:
        print("❌ Error: No se especificó archivo de entrada")
        return False

    # Generar ruta de salida
    output_file = blend_file.replace(".blend", ".obj")

    # Verificar que sea un cambio real
    if output_file == blend_file:
        print("❌ Error: El archivo no tiene extensión .blend")
        return False

    print(f"📁 Entrada: {blend_file}")
    print(f"📝 Salida:  {output_file}")

    try:
        # Exportar a OBJ
        # Opciones recomendadas:
        # - export_selected=False: Exporta todos los objetos
        # - apply_modifiers=True: Aplica modificadores
        # - forward_axis='-Y': Ajusta eje frontal
        # - up_axis='Z': Eje vertical

        bpy.ops.wm.obj_export(
            filepath=output_file,
            export_selected=False,
            apply_modifiers=True,
            forward_axis='-Y',
            up_axis='Z'
        )

        # Verificar que el archivo se creó
        if os.path.exists(output_file):
            file_size = os.path.getsize(output_file)
            print(f"✅ Exportación exitosa ({file_size} bytes)")
            return True
        else:
            print("❌ Error: El archivo de salida no se creó")
            return False

    except Exception as e:
        print(f"❌ Error durante exportación: {e}")
        return False

def main():
    """Punto de entrada principal."""
    print("=" * 60)
    print("  Blender OBJ Exporter - Script de Conversión")
    print("=" * 60)

    success = export_to_obj()

    print("=" * 60)

    # Salir con código apropiado
    if success:
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()
