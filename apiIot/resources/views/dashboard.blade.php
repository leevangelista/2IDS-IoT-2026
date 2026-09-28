<!DOCTYPE html>
<html lang="pt-BR">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>Dashboard</title>

    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>

<body>

    <h1>Dashboard do Sensor {{ $id }}</h1>

    <h2>Histórico</h2>

    <table border="1">
        <thead>
            <tr>
                <th>ID</th>
                <th>Sensor</th>
                <th>Dado</th>
                <th>Unidade</th>
                <th>Data</th>
            </tr>
        </thead>

        <tbody id="tabelaMedidas">
        </tbody>
    </table>


    <h2>Gráfico</h2>

    <canvas id="graficoSensor"></canvas>


    <script>

        const sensorId = {{ $id }};

        let grafico;


        function carregarMedidas() {

            fetch(`/api/dashboard/${sensorId}`)

                .then(response => response.json())

                .then(medidas => {

                    atualizarTabela(medidas);

                    atualizarGrafico(medidas);

                })

                .catch(error => {

                    console.error('Erro ao buscar medidas:', error);

                });
        }


        function atualizarTabela(medidas) {

            const tabela = document.getElementById('tabelaMedidas');

            tabela.innerHTML = '';


            medidas.forEach(medida => {

                const linha = document.createElement('tr');

                linha.innerHTML = `
                    <td>${medida.id}</td>
                    <td>${medida.sensor.nome}</td>
                    <td>${medida.dado}</td>
                    <td>${medida.unidade_medida}</td>
                    <td>${medida.data}</td>
                `;

                tabela.appendChild(linha);

            });

        }


        function atualizarGrafico(medidas) {

            const labels = medidas
                .slice()
                .reverse()
                .map(medida => medida.data);

            const dados = medidas
                .slice()
                .reverse()
                .map(medida => parseFloat(medida.dado));


            if (grafico) {

                grafico.data.labels = labels;

                grafico.data.datasets[0].data = dados;

                grafico.update();

                return;
            }


            const ctx = document
                .getElementById('graficoSensor')
                .getContext('2d');


            grafico = new Chart(ctx, {

                type: 'line',

                data: {

                    labels: labels,

                    datasets: [{

                        label: medidas[0]?.sensor?.nome ?? 'Sensor',

                        data: dados,

                        borderWidth: 2,

                        tension: 0.3

                    }]

                },

                options: {

                    responsive: true,

                    scales: {

                        y: {

                            title: {

                                display: true,

                                text: medidas[0]?.unidade_medida ?? 'Valor'

                            }

                        },

                        x: {

                            title: {

                                display: true,

                                text: 'Data/Hora'

                            }

                        }

                    }

                }

            });

        }


        // Primeira carga
        carregarMedidas();


        // Atualiza a cada 5 segundos
        setInterval(carregarMedidas, 5000);

    </script>

</body>

</html>