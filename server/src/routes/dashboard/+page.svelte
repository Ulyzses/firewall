<script lang='ts'>
    type Device = {
        name: string;
        number: number;
        status: string;
        smoke: boolean;
    };

    let devices: Device[] = [
        {name: 'pips', number: 202111579, status: 'on', smoke: true},
        {name: 'lance', number: 201800866, status: 'on', smoke: false},
        {name: 'julia', number: 202108595, status: 'forced off', smoke: false},
        {name: 'renzo', number: 202100912, status: 'forced off', smoke: false},
        {name: 'marius', number: 202102371, status: 'off', smoke: false},
        {name: 'jaren', number: 202102804, status: 'off', smoke: false}
    ]
</script>

<!-- top bar -->
    <!-- FireWall name -->
    <!-- user icon -->
        <!-- logout -->
<!-- devices -->
    <!-- title -->
        <!-- add device -->
    <!-- device list -->
        <!-- smoke detected -->
        <!-- plug icon -->
        <!-- name, number -->
        <!-- status -->