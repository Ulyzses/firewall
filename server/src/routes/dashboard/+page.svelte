<script lang="ts">
  type Device = {
    name: string;
    number: number;
    status: string;
    smoke: boolean;
  };

  let devices: Device[] = [
    { name: 'pips', number: 202111579, status: 'on', smoke: true },
    { name: 'lance', number: 201800866, status: 'on', smoke: false },
    { name: 'julia', number: 202108595, status: 'forced off', smoke: false },
    { name: 'renzo', number: 202100912, status: 'forced off', smoke: false },
    { name: 'marius', number: 202102371, status: 'off', smoke: false },
    { name: 'jaren', number: 202102804, status: 'off', smoke: false }
  ];

  export let data;

  $: ({ user, supabase } = data);

  let addDeviceForm: HTMLFormElement;

  async function addDevice() {
    const formData = new FormData(addDeviceForm);
    const macAddress = formData.get('macAddress') as string;

    const { data, error } = await supabase
      .from('devices')
      .insert({
        user: user?.id,
        mac: macAddress
      })
      .select();

    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }
  }
</script>

<form bind:this={addDeviceForm} on:submit|preventDefault={addDevice}>
  <label for="macAddress">
    <input name="macAddress" type="text" />
  </label>
  <button>Add Device</button>
</form>

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
