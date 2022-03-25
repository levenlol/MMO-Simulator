import unreal
new_material_path = '/Game/Assets/PolygonFantasyKingdom/Materials/Mat_PolygonFantasyKingdom_Mat_01_A.Mat_PolygonFantasyKingdom_Mat_01_A'
asset_folder = '/Game/Assets/PolygonFantasyKingdom/Meshes/Weapons/'

load_material = unreal.EditorAssetLibrary.load_asset(new_material_path)

# setting material based on asset in a  content browser directory

list_assets = unreal.EditorAssetLibrary.list_assets(asset_folder, True, False)
for asset in list_assets:
    load_asset = unreal.EditorAssetLibrary.load_asset(asset)
    if load_asset.get_class() == unreal.StaticMesh.static_class():
        load_asset.set_material(0,load_material)
        unreal.EditorAssetLibrary.save_asset(asset)
