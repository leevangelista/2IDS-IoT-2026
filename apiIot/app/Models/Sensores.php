<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;

class Sensores extends Model
{
    protected $table = "sensores";

    protected $fillable = [
        'nome',
        'status'
    ];

    public function medidas()
    {
        return $this->hasMany(MedidasSensores::class, 'sensor_id');
    }
}