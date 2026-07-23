<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use App\Http\Controllers\MedidasSensoresApiController;

Route::get('/user', function (Request $request) {
    return $request->user();
})->middleware('auth:sanctum');


// estou no api.php
// rotas para a api de medidasSensores
Route::get('medidas',[MedidasSensoresApiController::class, 'listarApi']);
Route::post('medidas/add',[MedidasSensoresApiController::class, 'addApi']);