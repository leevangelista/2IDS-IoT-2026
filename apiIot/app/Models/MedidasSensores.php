<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;

class MedidasSensores extends Model
{
    protected $table = "medidas_sensores";

    protected $fillable = [
        'dado',
        'unidade_medida',
        'data',
        'sensor_id'
    ];

    public function sensor()
    {
        return $this->belongsTo(Sensores::class, 'sensor_id');
    }
}