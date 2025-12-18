bl_info = {
    "name": "Export Scene to JSON",
    "author": "Gemini & User",
    "version": (1, 4),
    "blender": (2, 80, 0),
    "location": "File > Export > Scene JSON",
    "description": "Exporte la scène complète et les meshes au format JSON (Fix BoxCollider Center)",
    "warning": "",
    "category": "Import-Export",
}

import bpy
import json
import mathutils
import os
from bpy_extras.io_utils import ExportHelper

# ------ AXIS CONVERSION ------
# Blender → System (X, Y, Z) = (X, Z, -Y)

def convert_vec(v):
    return [
        float(v[0]),   # X stays X
        float(v[2]),   # Z becomes Y
        float(-v[1])   # Y becomes -Z
    ]
    
def convert_scale(s):
    return [
        float(s[0]),   # X stays X
        float(s[2]),   # Z becomes Y
        float(s[1])    # Y becomes Z
    ]

def convert_quaternion(q):
    """Convert Blender quaternion to system X-right, Y-up, Z-forward(-)."""
    # Conversion matrix
    M = mathutils.Matrix((
        (1, 0,  0),
        (0, 0,  1),
        (0, -1, 0)
    ))

    # Quaternion -> Matrix
    R = q.to_matrix()

    # Apply conversion: M * R * M⁻¹
    R2 = M @ R @ M.inverted()

    # Back to quaternion
    q2 = R2.to_quaternion()

    return [float(q2.x), float(q2.y), float(q2.z), float(q2.w)]

# ------ MESH EXPORT LOGIC ------
def get_mesh_data(obj, depsgraph):
    if obj.type != 'MESH':
        return None

    # Use evaluated object to apply modifiers and get correct mesh data
    eval_obj = obj.evaluated_get(depsgraph)
    mesh = eval_obj.to_mesh()
    
    # Ensure tessellation for loop triangles
    mesh.calc_loop_triangles()

    vertices_flat = []
    indices_flat = []
    uvs_flat = []
    normals_flat = []

    for v in mesh.vertices:
        x, y, z = convert_vec(v.co)
        vertices_flat.extend([x, y, z])

        nx, ny, nz = convert_vec(v.normal)
        normals_flat.extend([nx, ny, nz])

    for tri in mesh.loop_triangles:
        indices_flat.extend([int(tri.vertices[0]), int(tri.vertices[1]), int(tri.vertices[2])])

    # UVs
    if len(mesh.uv_layers) > 0:
        uv_per_vertex = [[0.0, 0.0] for _ in range(len(mesh.vertices))]
        uv_layer = mesh.uv_layers.active.data
        for loop in mesh.loops:
            vidx = loop.vertex_index
            # Check if loop index is within range (safety check)
            if loop.index < len(uv_layer):
                uv = uv_layer[loop.index].uv
                uv_per_vertex[vidx] = [float(uv.x), float(uv.y)]
        for uv in uv_per_vertex:
            uvs_flat.extend(uv)
    else:
        uvs_flat = []

    # Clean up the temporary mesh
    eval_obj.to_mesh_clear()

    return {
        "vertices": vertices_flat,
        "indices": indices_flat,
        "uvs": uvs_flat,
        "normals": normals_flat
    }

# ------ OPERATOR ------
class ExportSceneJSON(bpy.types.Operator, ExportHelper):
    """Export Scene and Meshes to JSON"""
    bl_idname = "export_scene.json"
    bl_label = "Export Scene JSON"
    filename_ext = ".json"

    def execute(self, context):
        scene_data = {"objects": []}
        
        # Get dependency graph for evaluated data (modifiers, etc.)
        depsgraph = context.evaluated_depsgraph_get()

        for obj in context.scene.objects:
            item = {}
            item["name"] = obj.name
            
            # Detection des Box Colliders
            is_box_collider = obj.name.lower().startswith("box collider")
            
            if is_box_collider:
                item["type"] = "BoxCollider"
            else:
                item["type"] = obj.type
            
            # --- GESTION DE LA POSITION ---
            if is_box_collider:
                # Pour les BoxColliders, on calcule le CENTRE de la Bounding Box en World Space.
                # Cela corrige le problème si l'origine de l'objet n'est pas au centre de la géométrie.
                local_bbox_center = 0.125 * sum((mathutils.Vector(b) for b in obj.bound_box), mathutils.Vector())
                world_bbox_center = obj.matrix_world @ local_bbox_center
                item["position"] = convert_vec(world_bbox_center)
            else:
                # Pour les objets normaux, on garde l'origine (pivot)
                pos = obj.matrix_world.to_translation()
                item["position"] = convert_vec(pos)
            
            rot = obj.matrix_world.to_quaternion()
            item["rotation"] = convert_quaternion(rot)
            
            # --- GESTION DU SCALE ---
            if is_box_collider:
                # On utilise les dimensions visuelles pour définir l'échelle
                # CORRECTION : On ne multiplie PAS par scale_signs. 
                # Un collider doit toujours avoir une échelle positive pour éviter min > max dans le moteur.
                
                final_scale = [
                    abs(obj.dimensions[0]),
                    abs(obj.dimensions[1]),
                    abs(obj.dimensions[2])
                ]
                
                item["scale"] = convert_scale(final_scale)
            else:
                scale = obj.matrix_world.to_scale()
                item["scale"] = convert_scale(scale)
            
            item["parent"] = obj.parent.name if obj.parent else None
            
            if is_box_collider:
                item["material"] = None
                item["mesh"] = None
            else:
                item["material"] = obj.active_material.name if obj.active_material else None
                item["mesh"] = get_mesh_data(obj, depsgraph)
            
            scene_data["objects"].append(item)

        # Write to the file selected in the file browser
        with open(self.filepath, "w", encoding='utf-8') as f:
            json.dump(scene_data, f, indent=4)

        self.report({'INFO'}, f"Exported: {self.filepath}")
        return {'FINISHED'}

# ------ REGISTRATION ------
def menu_func_export(self, context):
    self.layout.operator(ExportSceneJSON.bl_idname, text="Scene JSON (.json)")

def register():
    bpy.utils.register_class(ExportSceneJSON)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    bpy.utils.unregister_class(ExportSceneJSON)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()