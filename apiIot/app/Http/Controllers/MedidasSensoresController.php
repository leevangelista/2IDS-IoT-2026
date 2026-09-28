<?php
namespace App\Http\Controllers;
use App\Models\Sensores;
use App\Models\MedidasSensores;

use Illuminate\Http\Request;

class MedidasSensoresController extends Controller
{

    public function listarDashboard($id){

        return view('dashboard', compact('id'));
    }
}