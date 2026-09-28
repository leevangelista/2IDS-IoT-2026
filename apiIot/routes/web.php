<?php

use Illuminate\Support\Facades\Route;
use App\Http\Controllers\MedidasSensoresController;

Route::get('/', function () {
    return view('welcome');
});

Route::get('/dashboard/{id}', [MedidasSensoresController::class, 'listarDashboard'])
    ->name('dashboard');
