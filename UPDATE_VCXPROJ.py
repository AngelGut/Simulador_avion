import xml.etree.ElementTree as ET

# Leer el archivo .vcxproj
vcxproj_path = r"C:\Users\angel\Desktop\Universidad\8vo\Computacion Grafica\Proyectos\Proyecto 2\Simulador_avion\Simulador_avion\Simulador_avion.vcxproj"

# Namespaces
ns = {'msbuild': 'http://schemas.microsoft.com/developer/msbuild/2003'}
ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')

tree = ET.parse(vcxproj_path)
root = tree.getroot()

# Rutas que necesitamos
local_include = r"C:\Users\angel\Desktop\Universidad\8vo\Computacion Grafica\Proyectos\Proyecto 2\Simulador_avion\Simulador_avion\include"
vcpkg_include = r"C:\Users\angel\Desktop\varios\vcpkg\installed\x64-windows\include"
vcpkg_lib = r"C:\Users\angel\Desktop\varios\vcpkg\installed\x64-windows\lib"

# Incluir path local
local_lib = r"C:\Users\angel\Desktop\Universidad\8vo\Computacion Grafica\Proyectos\Proyecto 2\Simulador_avion\Simulador_avion\lib"

# Actualizar todas las configuraciones
for config in ['Debug|Win32', 'Release|Win32', 'Debug|x64', 'Release|x64']:
    # Buscar ClCompile
    for item_def in root.findall('.//msbuild:ItemDefinitionGroup[@Condition]', ns):
        if config in item_def.get('Condition', ''):
            # Actualizar o crear ClCompile
            cl_compile = item_def.find('msbuild:ClCompile', ns)
            if cl_compile is not None:
                add_include = cl_compile.find('msbuild:AdditionalIncludeDirectories', ns)
                if add_include is not None:
                    add_include.text = f"{local_include};{vcpkg_include};%(AdditionalIncludeDirectories)"
                else:
                    el = ET.Element('{http://schemas.microsoft.com/developer/msbuild/2003}AdditionalIncludeDirectories')
                    el.text = f"{local_include};{vcpkg_include};%(AdditionalIncludeDirectories)"
                    cl_compile.append(el)
            
            # Actualizar Link
            link = item_def.find('msbuild:Link', ns)
            if link is not None:
                add_lib_dir = link.find('msbuild:AdditionalLibraryDirectories', ns)
                if add_lib_dir is not None:
                    add_lib_dir.text = f"{local_lib};{vcpkg_lib};%(AdditionalLibraryDirectories)"
                else:
                    el = ET.Element('{http://schemas.microsoft.com/developer/msbuild/2003}AdditionalLibraryDirectories')
                    el.text = f"{local_lib};{vcpkg_lib};%(AdditionalLibraryDirectories)"
                    link.append(el)

tree.write(vcxproj_path, encoding='utf-8', xml_declaration=True)
print(f"✓ .vcxproj actualizado: {vcxproj_path}")
print(f"  Include: {vcpkg_include}")
print(f"  Library: {vcpkg_lib}")
