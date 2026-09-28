<?php
namespace App\Http\Controllers;
use App\Models\Sensores;
use App\Models\MedidasSensores;

use Illuminate\Http\Request;

class MedidasSensoresApiController extends Controller
{

    public function listarDashboard($id){
        try {

        $medidasSensores = MedidasSensores::with('sensor')
            ->where('sensor_id', $id)
            ->orderBy('data', 'desc')
            ->get();

        return response()->json($medidasSensores, 200);

        } catch (\Exception $e) {

            return response()->json([
                'message' => 'Erro interno do servidor',
                'erro' => $e->getMessage()
            ], 500);
        }
    }

    public function listarApi(Request $request){
        try {
            $query = MedidasSensores::with('sensor');

            $medidasSensores = $query->get();

            return response()->json($medidasSensores, 200);
    
        } catch (\Exception $e) {
            return response()->json([
                'message' => 'Erro interno do servidor',
                'erro' => $e->getMessage() // remova em produção
            ], 500);
        }
    }

    
    public function addApi(Request $request){

        try {
            $request->validate([
                'dado' => 'required|string|max:255',
                'unidade_medida' => 'required|string|max:255',
                'sensor_id' => 'required|exists:sensores,id'
            ]);

            $data = $request->data ?? now(); // se não enviar a data pegar a hora de agora
    
            $medidaSensor = MedidasSensores::create([
                'dado' => $request->dado,
                'unidade_medida' => $request->unidade_medida,
                'data' => $data,
                'sensor_id' => $request->sensor_id
            ]);
    
            return response()->json([
                'success' => true,
                'message' => 'Medida Sensor Adicionada',
                'data' => $medidaSensor
            ], 201);
    
        } catch (\Illuminate\Validation\ValidationException $e) {
    
            return response()->json([
                'success' => false,
                'message' => 'Erro de validação',
                'errors' => $e->errors()
            ], 422);
    
        } catch (\Exception $e) {
    
            return response()->json([
                'success' => false,
                'message' => 'Erro interno do servidor',
                'error' => $e->getMessage() // remova em produção
            ], 500);
        }
    }
}